/**
 * The LS (Logical Sugar), available under the following license:
 * Copyright (c) 2021 Nurimanshin Denis Radikovich.
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

namespace ls{

    template<class T>
    class Pointer{
    private:
        Memory::Header* header = nullptr;

        inline T* tp(){
            return (T*)(header+1);
        }
    public:
        template<typename... Args>
        bool create(Args... args){
            clear();
            header = LocalMemory->allocData(sizeof(T));
            if(header == nullptr){
                return false;
            }
            header->pointers++;
            new (tp()) T(args...);
            return true;
        }
        Pointer(){}
        ~Pointer(){
            clear();
        }

        Pointer(Pointer&& p) = delete;
        Pointer& operator=(Pointer&& p) = delete;

        Pointer(Pointer& p){
            if(p.header == nullptr){
                return;
            }
            header = p.header;
            header->pointers++;
        }

        Pointer& operator=(Pointer& p){
            if(this == &p){
                return*this;
            }
            clear();
            if(p.header == nullptr){
                return*this;
            }
            header = p.header;
            header->pointers++;
            return *this;
        }

        T* operator->(){
            return tp();
        }

        Memory::Header* release(){
            if(header == nullptr){
                return nullptr;
            }
            if(header->pointers == 1){
                Memory::Header* h = header;
                header = nullptr;
                h->pointers--;
                LocalMemory->releaseData(h);
                return h;
            }
            clear();
            return nullptr;
        }

        void capture(Memory::Header* h){
            if(h == nullptr){
                return;
            }
            clear();
            header = h;
            header->pointers++;
            LocalMemory->captureData(header);
        }

        void clear(){
            if(header == nullptr){
                return;
            }
            header->pointers--;
            if(header->pointers == 0){
                tp()->~T();
                LocalMemory->freeData(header);
            }
            header = nullptr;
        }

        inline bool isNull(){
            return header == nullptr;
        }
        inline bool isNotNull(){
            return header != nullptr;
        }
    };


    template<class To, class From>
    Pointer<To>& staticPointerCast(Pointer<From>& from){
        return *(Pointer<To>*)&from;
    }
}
