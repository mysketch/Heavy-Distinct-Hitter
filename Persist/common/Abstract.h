#ifndef ABSTRACT_H
#define ABSTRACT_H

#include <unordered_map>
#include <random>

#include "hash.h"

template<typename DATA_TYPE,typename COUNT_TYPE>
class Abstract{
public:

    struct Pair{
        uint8_t str[sizeof(DATA_TYPE) + sizeof(COUNT_TYPE)];

        void Set(const DATA_TYPE item, const COUNT_TYPE window){
            memcpy(str, &item, sizeof(DATA_TYPE));
            memcpy(str + sizeof(DATA_TYPE), &window, sizeof(COUNT_TYPE));
        }

        inline uint32_t hash(uint32_t seed = 0){
            return Hash::BOBHash32(str, sizeof(DATA_TYPE) + sizeof(COUNT_TYPE), seed);
        }
    };

    Abstract(){}
    virtual ~Abstract(){};

    virtual void Insert(Pair& pair, const DATA_TYPE item, const COUNT_TYPE window) = 0;
    virtual COUNT_TYPE Query(const DATA_TYPE item) = 0;
    virtual void NewWindow(const COUNT_TYPE window) = 0;

    inline uint32_t hash(DATA_TYPE data, uint32_t seed = 0){
        return Hash::BOBHash32((uint8_t*)&data, sizeof(DATA_TYPE), seed);
    }
};

#endif
