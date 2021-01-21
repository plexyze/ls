//
// Created by d on 20.01.2021.
//

#pragma once


#include "message.h"

namespace ls{
    class MessageLogError: public Message{
    public:
        stringstream msg;

        virtual MessageType getType() {
            return MESSAGE_LOG_ERROR;
        }

        virtual ~MessageLogError() {}

    public:

    };




}
