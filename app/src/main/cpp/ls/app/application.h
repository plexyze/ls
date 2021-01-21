//
// Created by d on 16.12.2020.
//

#ifndef LS_APPLICATION_H
#define LS_APPLICATION_H

#include <memory>
#include "../ui/renderer.h"

namespace ls{

    class Application {
    public:
        static std::shared_ptr<Renderer> renderer;

    };

}




#endif //LS_APPLICATION_H
