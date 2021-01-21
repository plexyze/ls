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

#include <mutex>
#include <vector>
#include "default_id.h"
#include "stopwatch.h"

using namespace std;

namespace ls{

    class Job{
        friend class ThreadPool;
    private:
        double time = 0.0;
    public:
        double getTime(){return time;}
        virtual void asyncUpdate()=0;
    public:
        virtual ~Job(){}
    };

    class ThreadPool{
    private:
        static mutex access;
        static condition_variable cvAccess;
        static mutex m;
        static vector<Job*>* jobRun;
        static condition_variable cvWait;
        static int thrCount;
        static int thrDefine;
        static int thrLock;
        static int cursor;
        static Stopwatch stopwatch;
        static double time;
    private:
        static void core();
    public:
        static void init(int threads = 0);

        static void run(vector<Job*>& jobs);

        static int getThreads();
    };


}

#include "thread_pool.inl"