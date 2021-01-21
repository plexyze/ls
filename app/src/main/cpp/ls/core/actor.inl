/**
 * The LS (Logical Sugar), available under the following license:
 * Copyright (c) 2020-2021 Nurimanshin Denis Radikovich.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

namespace ls{

    //Actor=========================================================================================
    SandboxContext* Actor::sandboxContext = nullptr;
    uint64_t Actor::getId() {
        return (uint64_t)this;
    }

    void Actor::repeat() {
        isRepeat = true;
    }

    void Actor::asyncUpdate() {
        Pointer<Message> msg;
        while(receive.size()>0){
            receive.pop(msg);
            onReceive(msg);
        }
        while(error.size()>0){
            error.pop(msg);
            onError(msg);
        }
        msg.clear();
        onUpdate();
        int eventCount = 0;

        //EVENT_SEND
        if(!send.empty()){
            events[eventCount++]=EVENT_SEND;
        }

        //EVENT_MEMORY
        memoryDiffSize = memory.getDiffSize(memorySizeRequired,sandboxContext->getMaxMemoryDiffSize());
        if(memoryDiffSize != 0){
            events[eventCount++]=EVENT_MEMORY;
        }

        //EVENT_FINISH
        if(!isRepeat){
            events[eventCount++]=EVENT_FINISH;
        }
        isRepeat = false;
        //end
        events[eventCount++]=EVENT_NON;
    }

    template<class T>
    void Actor::sendMessage(Pointer <T> &msg) {
        send.push(staticPointerCast<Message>(msg));
    }

    int64_t Actor::getMemoryMaxSize() {
        return memory.getMaxSize();
    }
    int64_t Actor::getMemoryFreeSize() {
        return memory.freeSize();
    }

    void Actor::setLocalPointers() {
        LocalMemory = &memory;
    }

    //WorkGroup=====================================================================================
    void WorkGroup::add(Actor *actor) {
        actors.push_back(actor);
    }

    void WorkGroup::asyncUpdate() {
        for(auto a: actors){
            a->setLocalPointers();
            a->asyncUpdate();
        }
        actors.clear();
    }


    //ActorManager==================================================================================
    ActorManager::ActorManager() {
        setMaxCore(1);
    }
    ActorManager::~ActorManager() {
        for(auto ref :sandboxContext.actors){
            delete (Actor*)ref;
        }
        sandboxContext.actors.clear();
    }

    void ActorManager::addUpdate(Actor *actor) {
        if(actor->info.isUpdate){
            return;
        }
        update.push_back(actor);
        actor->info.isUpdate = true;
    }

    void ActorManager::delUpdate(int index) {
        Actor* actor = update[index];
        actor->info.isUpdate = false;
        update[index] = update.back();
        update.pop_back();
    }

    void ActorManager::updateSendMessage(Actor *sendActor) {
        Pointer<Message> msg;
        while(sendActor->send.size()>0){
            sendActor->setLocalPointers();
            sendActor->send.pop(msg);
            msg->from = sendActor;
            Actor* receiveActor = (Actor*) msg->to;
            if(sandboxContext.actors.count(receiveActor)){
                Memory::Header* header = msg.release();
                receiveActor->setLocalPointers();
                msg.capture(header);
                receiveActor->receive.push(msg);
                addUpdate(receiveActor);
            }else{
                sendActor->error.push(msg);
            }
        }
    }

    void ActorManager::updateMemorySize(Actor *actor) {
        sandboxContext.memoryAllocated += actor->memory.setDiffMaxSize(actor->memoryDiffSize);
    }

    void ActorManager::setSandboxContext() {
        Actor::sandboxContext = &sandboxContext;
    }

    bool ActorManager::setUiActor(ActorRef ref) {
        if(!sandboxContext.actors.count(ref)){
            return false;
        }
        sandboxContext.ui = ref;
        return true;
    }

    bool ActorManager::setLoggerActor(ActorRef ref) {
        if(!sandboxContext.actors.count(ref)){
            return false;
        }
        sandboxContext.logger = ref;
        return true;
    }
    bool ActorManager::setSandboxActor(ActorRef ref) {
        if(!sandboxContext.actors.count(ref)){
            return false;
        }
        sandboxContext.sandbox = ref;
        return true;
    }
    ActorRef ActorManager::getUiActor(){
        return sandboxContext.ui;
    }
    ActorRef ActorManager::getLoggerActor(){
        return sandboxContext.logger;
    }
    ActorRef ActorManager::getSandboxActor(){
        return sandboxContext.sandbox;
    }


    void ActorManager::setMaxCore(int coreCount) {
        if(coreCount<1){
            coreCount = 1;
        }

        while (workGroups.size() < coreCount){
            workGroups.push_back(new WorkGroup());
        }

        while (workGroups.size() > coreCount){
            delete workGroups.back();
            workGroups.pop_back();
        }
    }

    ActorRef ActorManager::add(Actor * actor) {
        actor->setLocalPointers();
        actor->onInit();
        addUpdate(actor);
        sandboxContext.actors.insert(actor);
        return actor;
    }

    bool ActorManager::notify(ActorRef ref) {
        if(sandboxContext.actors.count(ref) == 0){
            return false;
        }
        addUpdate((Actor*)ref);
        return true;
    }

    int ActorManager::coreUpdate() {
        int calculateCount = update.size();
        if(calculateCount == 0){
            return calculateCount;
        }
        for(int i=0; i<calculateCount; i++){
            Actor* actor = update[i];
            int coreIndex = actor->workGroupIndex;
            if(coreIndex < 0){
                coreIndex = i;
            }
            coreIndex %= workGroups.size();
            static_cast<WorkGroup*>(workGroups[coreIndex])->add(actor);
        }
        ThreadPool::run(workGroups);

        int i=0;
        while( i<update.size()){
            Actor* actor = update[i];
            int event = 0;
            int8_t* events = actor->events;
            while(events[event] != Actor::EVENT_NON){
                switch (events[event++]) {
                    case Actor::EVENT_SEND:
                        updateSendMessage(actor);
                        break;
                    case Actor::EVENT_MEMORY:
                        updateMemorySize(actor);
                        break;
                    case Actor::EVENT_FINISH:
                        if(actor->receive.empty()){
                            delUpdate(i);
                            i--;
                        }
                        break;
                }
            }
            events[0] = Actor::EVENT_NON;
            i++;
        }
        return calculateCount;
    }

    bool ActorManager::isUpdateEmpty() {
        return update.empty();
    }

    bool ActorManager::isUpdateNotEmpty() {
        return !update.empty();
    }




}