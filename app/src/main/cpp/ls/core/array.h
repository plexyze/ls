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
    class Array{
    private:

        Memory::Header* header = nullptr;
        inline T* tp(int index){
            return (T*)(header+1)+index;
        }
    public:

        Array(){}
        ~Array(){
            resize(0);
            setCapacity(0);
        }

        inline int size(){
            if(header == nullptr){
                return 0;
            }
            return header->pointers;
        }

        inline int getCapacity(){
            if(header == nullptr){
                return 0;
            }
            return header->size/sizeof(T);
        }

        inline bool upSize(){
            return resize(size()+1);
        }

        inline T& operator[](int index){
            return *tp(index);
        }
        inline T& back(){
            return *tp(size()-1);
        }

        inline void clear(){
            resize(0);
        }

        bool setCapacity(int cap){
            if(cap < size()){
                cap = size();
            }
            if(cap == 0){
                LocalMemory->freeData(header);
                header = nullptr;
                return true;
            }
            return LocalMemory->resizeData(header, cap * sizeof(T));;
        }

        bool resize(int newCount){
            if(getCapacity() < newCount){
                if(!setCapacity(newCount)){
                    return false;
                }
            }
            while(size() > newCount){
                tp( --header->pointers)->~T();
            }

            while(size() < newCount){
                new( tp(header->pointers++) ) T();
            }
            return true;
        }
    };
}

