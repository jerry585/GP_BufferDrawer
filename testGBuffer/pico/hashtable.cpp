/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/
#include "hashtable.hpp"
#include <assert.h>

HashTable::HashTable(int iniSize):
    buckets(iniSize),
    used(0),
    searchIt(buckets.begin())
{
    assert(iniSize > 0);
}

HashTable::~HashTable() {
    clear();
}

HashTable::Entry * HashTable::find(Key key) {
    EntryVect &entries = buckets[slot(key)].entries;
    for (Entry *it = entries.begin(), *end = entries.end();
         it != end; ++it) {
        if (key == it->key) {
            return it;
        }
    }
    return 0;
}

bool HashTable::find(Key key, Value *val) {
    Entry *p = find(key);
    if (!p) {
        return false;
    } else {
        *val = p->value;
        return true;
    }
}

bool HashTable::erase(Key key) {
    EntryVect &entries = buckets[slot(key)].entries;
    for (Entry *it = entries.begin(), *end = entries.end();
         it != end; ++it) {
        if (key == it->key) {
            Entry *last = end - 1;
            if (it != last) {
                *it = *last;
            }
            entries.pop_back();
            --used;
            if (used < buckets.size() * kFillFactor / 4 &&
                buckets.size() > 1) {
                resize(buckets.size() >> 1);
            }
            return true;
        }
    }
    return false;
}

bool HashTable::insert(Key key, Value val) {
    if (used > buckets.size() * kFillFactor) {
        resize(buckets.size() << 1);
    }
    EntryVect &entries = buckets[slot(key)].entries;
    for (Entry *it = entries.begin(), *end = entries.end();
         it != end; ++it) {
        if (key == it->key) {
            it->value = val;
            return false;
        }
    }
    entries.push_back(Entry(key, val));
    ++used;
    return true;
}

void HashTable::clear() {
    for (Bucket *buckit = buckets.begin(), *buckend = buckets.end();
         buckit != buckend; ++buckit) {
        buckit->entries.clear();
    }
    used = 0;
    //resize(2);
}

void HashTable::resize(int newSize) {
    assert(newSize > 0);
    HashTable tmp(newSize);
    for (Bucket *buckit = buckets.begin(), *buckend = buckets.end();
         buckit != buckend; ++buckit) {
        EntryVect &entries = buckit->entries;
        for (Entry *it = entries.begin(), 
                 *end = entries.end();
             it != end; ++it) {
            tmp.insert(it->key, it->value);
        }
        entries.clear();
    }
    buckets.swap(tmp.buckets);
    searchIt = buckets.begin();
}

void HashTable::getAllEntries(EntryVect &outEntries) {
    outEntries.clear();
    for (Bucket *buckit = buckets.begin(), *buckend = buckets.end();
         buckit != buckend; ++buckit) {
        EntryVect &entries = buckit->entries;
        for (Entry *it = entries.begin(), 
                 *end = entries.end();
             it != end; ++it) {
            outEntries.push_back(*it);
        }
    }
    assert(outEntries.size() == used);
}

HashTable::Entry * HashTable::findSliceForAlloc(const int reqSize) {
    static const unsigned kLimit = 50;
    const unsigned limit = (kLimit < used) ? kLimit : used;
    if (!limit) {
        return 0;
    }
    unsigned seen = 0;
    Entry *bestEntry = 0;
    Bucket *searchItEnd = buckets.end();
    while(true) {
        EntryVect &entries = searchIt->entries;
        for (Entry *it = entries.begin(), *end = entries.end();
             it != end; ++it, ++seen) {
            if (seen >= limit) {
                return bestEntry;
            }            
            if (it->value >= reqSize &&
                (!bestEntry || it->key < bestEntry->key)) {
                bestEntry = it;
            }   
        }
        ++searchIt;
        if (searchIt == searchItEnd) {
            searchIt = buckets.begin();
        }
    }
    assert(false);
}
