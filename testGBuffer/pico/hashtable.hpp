/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _SLICEHASH_H_
#define _SLICEHASH_H_

#include "vector.hpp"
#include "gen.hpp"


#define HashTable giseHashTable

class HashTable {
public:
    typedef int64 Key;
    typedef int64 Value;
    struct Entry {
        Entry(Key iniKey, Value iniValue) :
            key(iniKey),
            value(iniValue)
        {}
        
        Key key;
        Value value;
    };
    typedef Vector<Entry> EntryVect;    

    HashTable(int iniSize = 2);
    ~HashTable();

    Entry * find(Key key);
    bool find(Key key, Value *val);
    bool erase(Key key);
    bool insert(Key key, Value val);
    void clear();
    Entry *findSliceForAlloc(const int size);
    void getAllEntries(EntryVect &outEntries);
    int size() { return used; }

private:
    //disallow copy
    HashTable(const HashTable &);
    void operator=(const HashTable &);

    unsigned slot(Key key) {
        return ((unsigned)key + (unsigned)(key >> 32) * 1023) & 
            (buckets.size() - 1);
    }

    void resize(int newSize);
    enum { kFillFactor = 8 }; 
    struct Bucket {
        EntryVect entries;
    };
    typedef Vector<Bucket> BucketVect;
    BucketVect buckets;
    unsigned used;
    Bucket *searchIt; //used by findSliceForAlloc()
};


#endif
