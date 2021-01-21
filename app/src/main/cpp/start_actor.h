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
        void onInit() override {}

        void onReceive(Pointer<Message> &msg) override {
            switch (msg->getType()) {
                case MESSAGE_LOG_ERROR:
                    onReceiveMessage(staticPointerCast<MessageLogError>(msg));
                    break;
            }
        }

        void onReceiveMessage(Pointer<MessageLogError>& error){
            error->msg << " loggerFreeSize: " << LocalMemory->freeSize()<<"/"<<LocalMemory->getMaxSize() << endl;
            log->e("T",error->msg);
        }
    };

    class TestUIActor: public UIActor{
    private:
        int i = 0;
    public:
        TestUIActor(){

        }

    protected:
        void onInit() override {
            memorySizeRequired = 256;
        }


    public:
        void onUpdateUI() override {
            i++;
            if(i%300 == 1){
                Pointer<MessageLogError> msg;
                if(msg.create()){
                    msg->to = getSandboxContext().getLogger();
                    msg->msg << " uiUpdate: "<< getId() << " freeSize: " << LocalMemory->freeSize()<<"/"<<LocalMemory->getMaxSize()<< endl;
                    sendMessage(msg);
                }
            }
        }

        virtual ~TestUIActor(){}
    };

    class TestSandboxActor: public SandboxActor{
    private:
        Log* log = nullptr;
        bool firstUpdate = true;
    public:
        TestSandboxActor(Log* log):log(log){

        }
    protected:
        void onUpdateSandbox(ActorManager &manager) override {
            if(firstUpdate){
                firstUpdate = false;
                ActorRef actorLogger = manager.add(new ActorLogger(log) );
                ActorRef actorUi = manager.add(new TestUIActor() );
                manager.setLoggerActor(actorLogger);
                manager.setUiActor(actorUi);
            }
        }
    };

}
