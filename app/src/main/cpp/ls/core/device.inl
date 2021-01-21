


namespace ls{

    mutex Device::m;
    condition_variable Device::cvWait;

    shared_ptr<Log> Device::log;
    vector<shared_ptr<Sandbox>> Device::sandboxes;

    void Device::calcUpdate() {
        bool exit = false;
        while(!exit){
            unique_lock<mutex> guard(m);
            int needUpdate = 0;

            for(auto &sandbox:sandboxes){
                needUpdate |=sandbox->calcUpdate();
            }

            if(needUpdate == 0){
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

        thread thr(calcUpdate);
        thr.detach();
    }

    void Device::startActor(SandboxActor *&& sandboxActor) {
        shared_ptr<Sandbox> sb(new Sandbox(move(sandboxActor)));
        sandboxes.push_back(sb);
    }

    void Device::uiUpdate() {
        unique_lock<std::mutex> guard(m);
        for(auto &sandbox:sandboxes){
            sandbox->uiUpdate();
        }
        cvWait.notify_all();
    }




}