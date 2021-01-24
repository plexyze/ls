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
    uint64_t Actor::getId() {
        return (uint64_t)this;
    }

    void Actor::repeat() {
        isRepeat = true;
    }

    void Actor::asyncUpdate() {
        changeMaxSize(availableMemory);
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
        onAsyncUpdate();
        changeMaxSize(availableMemory);
        isRepeat = isRepeat || getDiffSize()!=0 || send.isNotEmpty() || availableMemory != getMaxSize();
    }

    template<class T>
    void Actor::sendMessage(Pointer <T> &msg) {
        send.push(staticPointerCast<Message>(msg));
    }

    //WorkGroup=====================================================================================
    void WorkGroup::add(Actor *actor) {
        actors.push_back(actor);
    }

    void WorkGroup::asyncUpdate() {
        for(auto a: actors){
            THREAD_LOCAL_ACTOR_MEMORY = a;
            a->asyncUpdate();
        }
        actors.clear();
    }


    //ActorManager==================================================================================
    unordered_set<ActorRef> ActorManager::actors;
    vector<Actor*> ActorManager::update;
    vector<Job*> ActorManager::workGroups;
    int ActorManager::workGroupsCount = 16;

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
            THREAD_LOCAL_ACTOR_MEMORY = sendActor;
            sendActor->send.pop(msg);
            msg->from = sendActor;
            Actor* receiveActor = (Actor*) msg->to;
            if(actors.count(receiveActor)){
                ActorMemory::Header* header = msg.release();
                THREAD_LOCAL_ACTOR_MEMORY = receiveActor;
                msg.capture(header);
                receiveActor->receive.push(msg);
                addUpdate(receiveActor);
            }else{
                sendActor->error.push(msg);
            }
        }
    }

    void ActorManager::updateWorkCroupCount() {
        while (workGroups.size() < workGroupsCount){
            workGroups.push_back(new WorkGroup());
        }

        while (workGroups.size() > workGroupsCount){
            delete workGroups.back();
            workGroups.pop_back();
        }
    }

    void ActorManager::setWorkCroupCount(int wgCount) {
        if(wgCount<1){
            wgCount = 1;
        }
        workGroupsCount = wgCount;
    }

    template<class T, typename... Args>
    ActorRef ActorManager::createActor(Args... args) {
        if(sizeof(T) > GlobalMemory::getFreeSize()){
            return nullptr;
        }
        Actor* actor = new T(args...);
        actor->info.size = sizeof(T);
        changeMemoryCurrSize(sizeof(T));
        ActorMemory* old = THREAD_LOCAL_ACTOR_MEMORY;
        THREAD_LOCAL_ACTOR_MEMORY = actor;
        actor->onInit();
        THREAD_LOCAL_ACTOR_MEMORY = old;
        actors.insert(actor);
        addUpdate(actor);
        return actor;
    }

    bool ActorManager::updateActor(ActorRef ref){
        if(actors.count(ref) == 0){
            return false;
        }
        Actor* actor = static_cast<Actor*>(ref);
        ActorMemory* old = THREAD_LOCAL_ACTOR_MEMORY;
        THREAD_LOCAL_ACTOR_MEMORY = actor;
        actor->onUpdate();
        THREAD_LOCAL_ACTOR_MEMORY = old;
        addUpdate(actor);
        return true;
    }

    int ActorManager::actorManagerUpdate() {
        if(actors.size() == 0){
            setMemoryMaxDiffSize(0);
        }
        setMemoryMaxDiffSize(GlobalMemory::getMaxSize()/actors.size());

        updateWorkCroupCount();
        int calculateCount = update.size();
        if(calculateCount == 0){
            return 0;
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
            updateSendMessage(actor);
            takeDiffActorMemory(actor);
            bool isExit = !actor->isRepeat && actor->receive.isEmpty();
            actor->isRepeat = false;
            if(isExit){
                delUpdate(i);
                continue;
            }
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