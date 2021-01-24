//
// Created by d on 21.01.2021.
//

#pragma once

#include "ls/core/device.h"
#include "ls/core/message_log.h"
#include "ls/device/log.h"


using namespace std;

namespace ls{

    class ActorLogger: public Actor{
    private:
        Log* log = nullptr;
    public:
        ~ActorLogger() override {}
        ActorLogger(Log* log):log(log){

        }
    protected:
        void onInit() override {
            availableMemory = 0;
        }

        void onReceive(Pointer<Message> &msg) override {
            switch (msg->getType()) {
                case MESSAGE_LOG_ERROR:
                    onReceiveMessage(staticPointerCast<MessageLogError>(msg));
                    break;
            }
        }

        void onReceiveMessage(Pointer<MessageLogError>& error){
            error->msg << " loggerFreeSize: " << THREAD_LOCAL_ACTOR_MEMORY->freeSize()<<"/"<<THREAD_LOCAL_ACTOR_MEMORY->getMaxSize() << endl;
            log->e("T",error->msg);
        }

    };

    class TestUIActor: public Actor{
    private:
        int i = 0;
    public:
        TestUIActor(){

        }

    protected:
        void onInit() override {
            availableMemory = 256;
        }


    public:
        void onUpdate() override {
            i++;
            if(i%300 == 1){
                Pointer<MessageLogError> msg;
                if(msg.create()){
                    msg->to = Sandbox::getLoggerActor();
                    msg->msg << " uiUpdate: "<< getId() << " freeSize: " << THREAD_LOCAL_ACTOR_MEMORY->freeSize()<<"/"<<THREAD_LOCAL_ACTOR_MEMORY->getMaxSize()<<"/"<<GlobalMemory::getSize()<< endl;
                    sendMessage(msg);
                }
            }
        }

        virtual ~TestUIActor(){}
    };

    class TestSandboxActor: public Actor{
    private:
        Log* log = nullptr;
        bool firstUpdate = true;
    public:
        TestSandboxActor(Log* log):log(log){

        }
    protected:
        void onUpdate() override {
            if(firstUpdate){
                firstUpdate = false;
                ActorRef loggerRef = ActorManager::createActor<ActorLogger>(log);
                Sandbox::setLoggerActor(loggerRef);
                ActorRef uiRef = ActorManager::createActor<TestUIActor>();
                Sandbox::setUiActor(uiRef);
            }
        }
    };

}
