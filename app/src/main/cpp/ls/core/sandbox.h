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

#include <unordered_map>
#include <unordered_set>
#include "default_id.h"
#include "message.h"
#include "actor.h"

using namespace std;

namespace ls{

    class Sandbox{
        static const int MAX_UPDATE = 1000;
    public:
        static ActorRef uiActorRef;
        static ActorRef loggerActorRef;
        static ActorRef sandboxActorRef;
    public:

        static bool setUiActor(ActorRef ref);
        static bool setLoggerActor(ActorRef ref);
        static bool setSandboxActor(ActorRef ref);

        static ActorRef getUiActor(){ return uiActorRef; }
        static ActorRef getLoggerActor(){ return loggerActorRef; }
        static ActorRef getSandboxActor(){ return sandboxActorRef; }

        static void uiUpdate();

        static bool calcUpdate();

    };




}

#include "sandbox.inl"