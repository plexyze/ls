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


#include <thread>
#include "thread_pool.h"


namespace ls{

    mutex ThreadPool::access;
    condition_variable ThreadPool::cvAccess;
    mutex ThreadPool::m;
    condition_variable ThreadPool::cvWait;
    int ThreadPool::thrCount;
    int ThreadPool::thrDefine;
    int ThreadPool::thrLock;

    vector<Job*>* ThreadPool::jobRun;
    int ThreadPool::cursor;
    Stopwatch ThreadPool::stopwatch;
    double ThreadPool::time;

    void ThreadPool::core() {
        {
            unique_lock<std::mutex> guard(m);
            thrCount++;
        }
        Job* next = nullptr;
        while(true){
            {
                unique_lock<std::mutex> guard(m);
                if(thrCount > thrDefine){
                    thrCount --;
                    return;
                }
                if(!jobRun){
                    thrLock++;
                    cvWait.wait(guard);
                    thrLock--;
                    continue;
                }

                if(cursor < jobRun->size()){
                    next = jobRun->at(cursor++);
                    next->time = time;
                }

                if(!next){
                    thrLock++;
                    if(thrLock >= thrCount){
                        cvAccess.notify_all();
                    }
                    cvWait.wait(guard);
                    thrLock--;
                    continue;
                }
            }

            if(next){
                next->asyncUpdate();
                next = nullptr;
            }

        }

    }


    void ThreadPool::init(int threads) {
        std::unique_lock<std::mutex> guard(m);

        int hardware_concurrency = thread::hardware_concurrency();

        if(hardware_concurrency < 1){
            hardware_concurrency = 8;
        }

        if(threads > 0){
            thrDefine = threads;
        }else{
            thrDefine = hardware_concurrency;
        }

        for(int i =thrCount; i < thrDefine; i++){
            thread thr(core);
            thr.detach();
        }
        cvWait.notify_all();
    }

    int ThreadPool::getThreads() {
        std::unique_lock<std::mutex> guard(m);
        return thrCount;
    }

    void ThreadPool::run(vector<Job*> &jobs) {
        unique_lock<std::mutex> guardAccess(access);
        unique_lock<std::mutex> guard(m);
        jobRun = &jobs;
        time = stopwatch.elapsed();
        cursor = 0;
        cvWait.notify_all();
        while(cursor < jobs.size()){
            cvAccess.wait(guard);
        }
        jobRun = nullptr;
    }


}