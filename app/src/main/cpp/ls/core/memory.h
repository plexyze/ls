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

namespace ls{


    class GlobalMemory{
    private:
        friend class GlobalMemoryAccess;
    private:
        static int64_t maxDiffSize;
        static int64_t maxSize;
        static int64_t currSize;
    public:
        static inline int64_t getMaxDiffSize(){return maxDiffSize;}
        static inline int64_t getMaxSize(){return maxSize;}
        static inline int64_t getSize(){return currSize;}
        static inline int64_t getFreeSize(){return maxSize - currSize;}
    };

    class ActorMemory{
    private:
        template<class T>
        friend class Pointer;

        template<class T>
        friend class Array;

        friend class GlobalMemoryAccess;
    public:
        struct Header{
            int32_t size;
            int32_t pointers;
        };

        static const int HEADER_SIZE = sizeof(Header) + 16;

    private:
        int64_t diffSize = 0;
        int64_t maxSize = 0;
        int64_t currSize = 0;

    public:
        inline int64_t freeSize(){ return maxSize - currSize; }
        inline int64_t size(){ return currSize;}
        inline int64_t getMaxSize(){return maxSize;}
        inline int64_t getDiffSize(){return diffSize;}
    private:
        Header* allocData(int dataSize);

        void freeData(Header* header);

        void releaseData(Header* header);

        void captureData(Header* header);

        bool resizeData(Header* &header, int dataSize);

    protected:
        inline void changeMaxSize(int64_t requiredSize);

    public:


    };

    class GlobalMemoryAccess{
    protected:
        static inline void takeDiffActorMemory(ActorMemory* actorMemory){
            GlobalMemory::currSize += actorMemory->diffSize;
            actorMemory->diffSize = 0;
        }

        static inline void setMemoryMaxSize(int64_t maxSize){
            if(maxSize<GlobalMemory::currSize){
                maxSize = GlobalMemory::currSize;
            }
            GlobalMemory::maxSize = maxSize;
        }

        static inline void setMemoryMaxDiffSize(int64_t diffSize){
            GlobalMemory::maxDiffSize = diffSize;
        }

        static inline void changeMemoryCurrSize(int64_t currSize){
            GlobalMemory::currSize += currSize;
        }

    };



    thread_local ActorMemory* THREAD_LOCAL_ACTOR_MEMORY;

    template<class T>
    class Pointer{
    private:
        ActorMemory::Header* header = nullptr;

        inline T* tp(){ return (T*)(header+1);}
    public:

        Pointer(){}
        ~Pointer(){clear();}
        inline bool isNull(){ return header == nullptr;}
        inline bool isNotNull(){return header != nullptr;}
        inline T* operator->(){return tp();}

        template<typename... Args>
        bool create(Args... args);

        Pointer(Pointer&& p) = delete;
        Pointer& operator=(Pointer&& p) = delete;

        Pointer(Pointer& p);

        Pointer& operator=(Pointer& p);

        ActorMemory::Header* release();

        void capture(ActorMemory::Header* h);

        void clear();
    };

    template<class To, class From>
    Pointer<To>& staticPointerCast(Pointer<From>& from){
        return *(Pointer<To>*)&from;
    }

    template<class T>
    class Array{
    private:
        ActorMemory::Header* header = nullptr;
        inline T* tp(int index){ return (T*)(header+1)+index;}
    public:

        Array(){}
        ~Array(){
            resize(0);
            setCapacity(0);
        }
        inline T& operator[](int index){ return *tp(index); }
        inline T& back(){ return *tp(size()-1); }
        inline void clear(){ resize(0); }

        inline int size();

        inline int getCapacity();

        inline bool upSize();

        bool setCapacity(int cap);

        bool resize(int newCount);
    };
}


#include "memory.inl"