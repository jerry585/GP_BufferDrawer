/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <assert.h>
#include <string.h>
#include <stdlib.h>

template <class T>
class Vector {
public:
    Vector(int iniSize):
        data((T*)calloc(sizeof(T), iniSize)),
        mSize(iniSize),
        mAllocSize(iniSize)
    {
        assert(iniSize > 0);
        assert(data);
    }

    Vector() :
        data(0),
        mSize(0),
        mAllocSize(0) {       
    }

    ~Vector() {
        clear();
    }

    void clear() {
        if (data) {
            free(data);
            data = 0;
        }
        mSize = 0;
        mAllocSize = 0;
    }

    void push_back(const T &elem) {
        assert(mSize <= mAllocSize);
        if (mSize == mAllocSize) {
            mAllocSize = (mAllocSize ? (mAllocSize << 1) : 2); 
            data = (T*)realloc(data, sizeof(T) * mAllocSize);
            assert(data);
        }
        assert(mSize < mAllocSize);
        data[mSize] = elem;
        ++mSize;
    }

    void pop_back() {
        assert(mSize > 0);
        --mSize;
        if (mSize < (mAllocSize >> 2)) {
            mAllocSize = (mAllocSize >> 1);
            data = (T*)realloc(data, sizeof(T) * mAllocSize);
        }
    }

    T &back() {
        assert(mSize > 0);
        return data[mSize - 1];
    }

    T * begin() {
        return data;
    }

    T * end() {
        return data + mSize;
    }

    T & operator[](int pos) {
        assert(pos >= 0 && pos < mSize);
        return data[pos];
    }

    unsigned size() {
        return mSize;
    }

    bool empty() {
        return mSize == 0;
    }

    void swap(Vector<T> &other) {
        Vector<T> save;
        save = other;
        other = *this;
        *this = save;
        save = Vector<T>();
        /*
        T *saveData       = other.data;
        int saveSize      = other.mSize;
        int saveAllocSize = other.mAllocSize;
        other.data = data;
        other.mSize = mSize;
        other.mAllocSize = mAllocSize;
        data       = saveData;
        mSize      = saveSize;
        mAllocSize = saveAllocSize;
        */
    }

private:
    //unsafe, only used by swap()
    void operator=(const Vector<T> &other) {
        memcpy(this, &other, sizeof(other));
    }
    
    Vector(const Vector<T> &other);// { assert(false); }

    T *data;
    int mSize;
    int mAllocSize;
};

#endif
