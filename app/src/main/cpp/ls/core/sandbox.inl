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

    ActorRef Sandbox::uiActorRef;
    ActorRef Sandbox::loggerActorRef;
    ActorRef Sandbox::sandboxActorRef;

    bool Sandbox::setUiActor(ActorRef ref) {
        if(!ActorManager::updateActor(ref)){
            return false;
        }
        uiActorRef = ref;
        return true;
    }

    bool Sandbox::setLoggerActor(ActorRef ref) {
        if(!ActorManager::updateActor(ref)){
            return false;
        }
        loggerActorRef = ref;
        return true;
    }
    bool Sandbox::setSandboxActor(ActorRef ref) {
        if(!ActorManager::updateActor(ref)){
            return false;
        }
        sandboxActorRef = ref;
        return true;
    }



    void Sandbox::uiUpdate() {
        ActorManager::updateActor(uiActorRef);
        ActorManager::updateActor(loggerActorRef);
    }

    bool Sandbox::calcUpdate() {
        int calculateCount = 0;
        while(calculateCount<MAX_UPDATE && ActorManager::isUpdateNotEmpty()){
            ActorManager::updateActor(sandboxActorRef);
            calculateCount += ActorManager::actorManagerUpdate();
        }
        return ActorManager::isUpdateNotEmpty();
    }




}