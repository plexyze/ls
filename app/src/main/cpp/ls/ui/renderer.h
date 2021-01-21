//
// Created by d on 16.12.2020.
//

#ifndef LS_RENDERER_H
#define LS_RENDERER_H


namespace ls{
    class Renderer {
    public:
        virtual void init() = 0;
        virtual void resize(int w, int h) = 0;
        virtual bool draw() = 0;
    };
}


#endif //LS_RENDERER_H
