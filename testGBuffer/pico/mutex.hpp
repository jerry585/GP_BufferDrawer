/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "os.hpp"


class Mutex {
public:
    Mutex() {
        os_mutex_init(mutex);
    }

    ~Mutex() {
        os_mutex_destroy(mutex);
    }

    void lock() {
        os_mutex_lock(mutex);
    }

    void unlock() {
        os_mutex_unlock(mutex);
    }

private:
    //deny copy
    Mutex(const Mutex &);
    void operator=(const Mutex &);

    os_pMutex mutex;
};

class Lock {
public:
    Lock(Mutex *iniMutex) :
        mutex(iniMutex)
    {
        mutex->lock();
    }
    
    ~Lock() {
        mutex->unlock();
    }

private:
    Mutex *mutex;
};



#endif
