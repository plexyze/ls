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

#include "stdlib.h"
#include "default_id.h"

namespace ls{

    class Memory{
    public:
        struct Header{
            int32_t size;
            int32_t pointers;
        };

        static const int HEADER_SIZE = sizeof(Header) + 16;

    private:
        int64_t maxSize = 0;
        int64_t currSize = 0;

    public:
        inline int64_t getDiffSize(int64_t requiredSize, int64_t maxMemorySize){
            int64_t diff = requiredSize - maxSize;
            int64_t minDiff = currSize - maxSize;
            if(diff > maxMemorySize){
                diff = maxMemorySize;
            }
            if(diff < minDiff){
                diff = minDiff;
            }
            return diff;
        }

        inline int64_t freeSize(){
            return maxSize - currSize;
        }
        inline int64_t size(){
            return currSize;
        }
        inline int64_t setDiffMaxSize(int64_t diff){
            int64_t minDiff = currSize - maxSize;
            if(diff < minDiff){
                diff = minDiff;
            }
            maxSize+=diff;
            return diff;
        }
        inline int64_t getMaxSize(){
            return maxSize;
        }
    public:

        Header* allocData(int dataSize){
            if(dataSize < 8){
                dataSize = 8;
            }
            if(dataSize + HEADER_SIZE > freeSize()){
                return nullptr;
            }
            Header* header = (Header*) malloc(dataSize + sizeof(Header));
            if(header == nullptr){
                return nullptr;
            }
            header->size = dataSize;
            header->pointers = 0;
            currSize+=(header->size+HEADER_SIZE);
            return header;
        }

        void freeData(Header* header){
            if(header== nullptr){
                return;
            }
            currSize -= (header->size+HEADER_SIZE);
            free(header);
        }

        void releaseData(Header* header){
            if(header == nullptr){
                return;
            }

            currSize -= (header->size+HEADER_SIZE);
            maxSize -= (header->size+HEADER_SIZE);
        }

        void captureData(Header* header){
            if(header == nullptr){
                return;
            }
            currSize += (header->size+HEADER_SIZE);
            maxSize += (header->size+HEADER_SIZE);
        }

        bool resizeData(Header* &header, int dataSize){
            if(header == nullptr){
                header = allocData(dataSize);
                return header!=nullptr;
            }
            if(dataSize < sizeof(Header)){
                dataSize = sizeof(Header);
            }
            if(dataSize - header->size > freeSize()){
                return false;
            }
            void* newPtr = realloc(header,dataSize+ sizeof(Header));

            if(newPtr == nullptr){
                return false;
            }
            header = (Header*)newPtr;
            currSize += (dataSize - header->size);
            header->size = dataSize;
            return true;
        }



    };

    thread_local Memory* LocalMemory;




}
