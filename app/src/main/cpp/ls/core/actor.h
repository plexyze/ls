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
#include "memory.h"
#include <unordered_set>

namespace ls{

    struct ActorManagerInfo{
        bool isUpdate = false;
        int size = 0;
    };

    class Actor: public ActorMemory, ActorRefClass{
        friend class ActorManager;
        friend class WorkGroup;
    private:
        void asyncUpdate();
    private:
        ActorManagerInfo info;
        bool isRepeat = false;
        MessagesQueue send;
        MessagesQueue receive;
        MessagesQueue error;
    protected:
        int64_t availableMemory = 1024;
        int workGroupIndex = -1;
        inline uint64_t getId();
        inline void repeat();

        template<class T>
        inline void sendMessage(Pointer<T>& msg);

        virtual void onInit() { };
        virtual void onReceive(Pointer<Message>& msg) { };
        virtual void onError(Pointer<Message>& msg) { };
        virtual void onAsyncUpdate(){}
        virtual void onUpdate(){}

    public:
        virtual ~Actor() {}
    };

    class WorkGroup: public Job{
    private:
        vector<Actor*> actors;
    public:
        void asyncUpdate() override;

        void add(Actor* actor);

        ~WorkGroup() override {}
    };

    class ActorManager: public GlobalMemoryAccess{
    private:
        static unordered_set<ActorRef> actors;
        static vector<Actor*> update;
        static vector<Job*> workGroups;
        static int workGroupsCount;
    private:
        static inline void updateSendMessage(Actor *actor);
        static inline void addUpdate(Actor* actor);
        static inline void delUpdate(int index);

        static void updateWorkCroupCount();;

    public:

        static void setWorkCroupCount(int wgCount);

        template<class T,typename... Args>
        static ActorRef createActor(Args... args);

        static bool updateActor(ActorRef ref);

        static int actorManagerUpdate();

        static inline bool isUpdateEmpty();
        static inline bool isUpdateNotEmpty();
    };




}

#include "actor.inl"
