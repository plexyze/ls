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

    mutex Device::m;
    condition_variable Device::cvWait;

    shared_ptr<Log> Device::log;

    void Device::calcUpdate() {
        bool exit = false;
        while(!exit){
            unique_lock<mutex> guard(m);
            if(!Sandbox::calcUpdate()){
                cvWait.wait(guard);
            }
        }
    }

    void Device::init(const shared_ptr<Log> &_log) {
        unique_lock<std::mutex> guard(m);
        static bool isInit = false;
        if(isInit){
            return;
        }
        isInit = true;
        log = _log;
        ThreadPool::init();
        setMemoryMaxSize(536870912);

        thread thr(calcUpdate);
        thr.detach();
    }

    void Device::uiUpdate() {
        unique_lock<std::mutex> guard(m);
        Sandbox::uiUpdate();
        cvWait.notify_all();
    }




}