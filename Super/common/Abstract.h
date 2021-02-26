#ifndef ABSTRACT_H
#define ABSTRACT_H

#include <unordered_map>
#include <unordered_set>
#include <random>
#include <string>

#include "hash.h"

class Abstract{
public:
    std::string NAME;
    uint64_t MEMORY;

    Abstract(){}
    virtual ~Abstract(){};

    virtual void Insert(uint64_t item, uint32_t src, uint32_t dst) = 0;
    virtual int32_t Query(uint32_t src) = 0;
    virtual uint32_t Report(uint32_t HIT) = 0;
};

#endif
