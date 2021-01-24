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

#include "memory.h"

using namespace std;

namespace ls{

    class ActorRefClass{

    };

    using ActorRef = ActorRefClass*;

    enum MessageType{
        MESSAGE_NON             = 0,
        MESSAGE_LOG_INFO        = 1,
        MESSAGE_LOG_ERROR       = 2,
    };

    class Message{
        friend class MessagesQueue;
    private:
        Pointer<Message> next;
    public:
        ActorRef from;
        ActorRef to;
    public:
        virtual MessageType getType()=0;
        virtual ~Message(){}
    };

    class MessagesQueue{
    private:
        Pointer<Message> last;
        Pointer<Message> first;
        int count = 0;
    public:
        inline bool isEmpty(){ return count == 0;}
        inline bool isNotEmpty(){ return count != 0;}
        inline int size(){
            return count;
        }
        void push(Pointer<Message>& msg){
            if(msg.isNull()){
                return;
            }
            if(last.isNull()){
                last = msg;
                first = msg;
            }else{
                last->next = msg;
                last = msg;
            }
            count++;
            msg.clear();
        }

        void pop(Pointer<Message>& msg){
            msg.clear();

            if(first.isNull()){
                return;
            }

            msg = first;
            first = first->next;
            msg->next.clear();
            count--;
            if(first.isNull()){
                last.clear();
            }
        }
    };



}