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

#pragma once

#include "thread_pool.h"
#include "default_id.h"
#include "message.h"
#include "sandbox.h"
#include "array.h"
#include "pointer.h"
#include <unordered_set>

namespace ls{

    struct ActorManagerInfo{
        bool isUpdate = false;
    };

    class SandboxContext{
        friend class ActorManager;
    private:
        unordered_set<ActorRef> actors;
        int64_t memorySizeMax = 268435456;
        int64_t memoryAllocated = 0;
        ActorRef ui = nullptr;
        ActorRef logger = nullptr;
        ActorRef sandbox = nullptr;
    public:
        inline int64_t getMaxMemoryDiffSize(){return getMemoryFreeSize() / actors.size();}
        inline int64_t getMemoryFreeSize(){return memorySizeMax - memoryAllocated;}
        inline int64_t getMemoryMaxSize(){return memorySizeMax;}
        inline int64_t getMemoryAllocated(){return memoryAllocated;}
        inline ActorRef getUi(){return ui;}
        inline ActorRef getLogger(){return logger;}
        inline ActorRef getSandbox(){return sandbox;}
    };

    class Actor: public ActorRefClass{
        friend class ActorManager;
        friend class WorkGroup;

    private:
        enum Event{
            EVENT_NON,
            EVENT_SEND,
            EVENT_MEMORY,
            EVENT_FINISH,
            MAX_EVENT
        };
    private:
        void asyncUpdate();

    private:
        static SandboxContext* sandboxContext;
        int8_t events[MAX_EVENT] = {EVENT_NON};
        ActorManagerInfo info;
        Memory memory;
        bool isRepeat = false;
        MessagesQueue send;
        MessagesQueue receive;
        MessagesQueue error;
        int64_t memoryDiffSize = 0;
    protected:
        int64_t memorySizeRequired = 256;
        int workGroupIndex = -1;

        inline int64_t getMemoryMaxSize();
        inline int64_t getMemoryFreeSize();
        inline uint64_t getId();
        inline SandboxContext& getSandboxContext(){return *sandboxContext;}
        inline void repeat();

        template<class T>
        inline void sendMessage(Pointer<T>& msg);

        virtual void onInit() { };
        virtual void onReceive(Pointer<Message>& msg) { };
        virtual void onError(Pointer<Message>& msg) { };
        virtual void onUpdate(){}

    public:
        virtual ~Actor() {}
        void setLocalPointers();
    };

    class WorkGroup: public Job{
    private:
        vector<Actor*> actors;
    public:
        void asyncUpdate() override;

        void add(Actor* actor);

        ~WorkGroup() override {}
    };



    class ActorManager{
    private:
        SandboxContext sandboxContext;
        vector<Actor*> update;
        vector<Job*> workGroups;
    private:
        inline void updateSendMessage(Actor *actor);
        inline void updateMemorySize(Actor *actor);
    private:

        inline void addUpdate(Actor* actor);
        inline void delUpdate(int index);

    public:
        void setSandboxContext();
        bool setUiActor(ActorRef ref);
        bool setLoggerActor(ActorRef ref);
        bool setSandboxActor(ActorRef ref);

        ActorRef getUiActor();
        ActorRef getLoggerActor();
        ActorRef getSandboxActor();

        inline int64_t  memoryFreeSize(){
            return  sandboxContext.memorySizeMax - sandboxContext.memoryAllocated;
        }
        inline int64_t  memoryAllocatedSize(){
            return sandboxContext.memoryAllocated;
        }
        ActorManager();
        ~ActorManager();

        void setMaxCore(int coreCount);

        ActorRef add(Actor* actor);

        bool notify(ActorRef ref);

        int coreUpdate();

        inline bool isUpdateEmpty();
        inline bool isUpdateNotEmpty();

    };




}

#include "actor.inl"
