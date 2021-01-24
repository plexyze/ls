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

    int64_t GlobalMemory::maxDiffSize = 0;
    int64_t GlobalMemory::maxSize = 0;
    int64_t GlobalMemory::currSize = 0;

    ActorMemory::Header *ActorMemory::allocData(int dataSize) {
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

    void ActorMemory::freeData(ActorMemory::Header *header) {
        if(header== nullptr){
            return;
        }
        currSize -= (header->size+HEADER_SIZE);
        free(header);
    }

    void ActorMemory::releaseData(ActorMemory::Header *header) {
        if(header == nullptr){
            return;
        }

        currSize -= (header->size+HEADER_SIZE);
        maxSize -= (header->size+HEADER_SIZE);
    }

    void ActorMemory::captureData(ActorMemory::Header *header) {
        if(header == nullptr){
            return;
        }
        currSize += (header->size+HEADER_SIZE);
        maxSize += (header->size+HEADER_SIZE);
    }

    bool ActorMemory::resizeData(ActorMemory::Header *&header, int dataSize) {
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

    void ActorMemory::changeMaxSize(int64_t requiredSize) {
        int64_t diff = requiredSize - maxSize;
        int64_t minDiff = currSize - maxSize;
        int64_t maxDiff = GlobalMemory::getMaxDiffSize() - diffSize;

        if(diff > maxDiff){
            diff = maxDiff;
        }

        if(diff < minDiff){
            diff = minDiff;
        }

        maxSize+=diff;
        diffSize+=diff;
    }

    //Pointer<T>
    template<class T>
    template<typename... Args>
    bool Pointer<T>::create(Args... args) {
        clear();
        header = THREAD_LOCAL_ACTOR_MEMORY->allocData(sizeof(T));
        if(header == nullptr){
            return false;
        }
        header->pointers++;
        new (tp()) T(args...);
        return true;
    }

    template<class T>
    Pointer<T>::Pointer(Pointer &p) {
        if(p.header == nullptr){
            return;
        }
        header = p.header;
        header->pointers++;
    }

    template<class T>
    Pointer<T> &Pointer<T>::operator=(Pointer &p) {
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

    template<class T>
    ActorMemory::Header *Pointer<T>::release() {
        if(header == nullptr){
            return nullptr;
        }
        if(header->pointers == 1){
            ActorMemory::Header* h = header;
            header = nullptr;
            h->pointers--;
            THREAD_LOCAL_ACTOR_MEMORY->releaseData(h);
            return h;
        }
        clear();
        return nullptr;
    }

    template<class T>
    void Pointer<T>::capture(ActorMemory::Header *h) {
        if(h == nullptr){
            return;
        }
        clear();
        header = h;
        header->pointers++;
        THREAD_LOCAL_ACTOR_MEMORY->captureData(header);
    }

    template<class T>
    void Pointer<T>::clear() {
        if(header == nullptr){
            return;
        }
        header->pointers--;
        if(header->pointers == 0){
            tp()->~T();
            THREAD_LOCAL_ACTOR_MEMORY->freeData(header);
        }
        header = nullptr;
    }

    //Array<T>
    template<class T>
    int Array<T>::size() {
        if(header == nullptr){
            return 0;
        }
        return header->pointers;
    }

    template<class T>
    int Array<T>::getCapacity() {
        if(header == nullptr){
            return 0;
        }
        return header->size/sizeof(T);
    }

    template<class T>
    bool Array<T>::upSize() {
        return resize(size()+1);
    }

    template<class T>
    bool Array<T>::setCapacity(int cap) {
        if(cap < size()){
            cap = size();
        }
        if(cap == 0){
            THREAD_LOCAL_ACTOR_MEMORY->freeData(header);
            header = nullptr;
            return true;
        }
        return THREAD_LOCAL_ACTOR_MEMORY->resizeData(header, cap * sizeof(T));;
    }

    template<class T>
    bool Array<T>::resize(int newCount) {
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

}