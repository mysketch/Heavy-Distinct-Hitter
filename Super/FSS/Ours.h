#ifndef OURS_H
#define OURS_H

#include "bitset.h"
#include "Abstract.h"

template<uint32_t SLOT_NUM>
class Ours : public Abstract {
public:

    struct Bucket {
        uint32_t items[SLOT_NUM];
        int32_t counters[SLOT_NUM];

        inline uint32_t Query(const uint32_t item){
            for(uint32_t i = 0;i < SLOT_NUM;++i){
                if(items[i] == item)
                    return counters[i];
            }
            return 0;
        }
    };

    Ours(uint64_t memory, uint32_t _K, double ratio) : K(_K),
            length((double)memory / (sizeof(Bucket))){
        MEMORY = memory;
        NAME = "Ours";

        length = ratio * memory / sizeof(Bucket);
        BF_length = (1 - ratio) * memory * 8;
        BF = new BitSet(BF_length);
        buckets = new Bucket[length];
        memset(buckets, 0, length * sizeof(Bucket));
    }

    ~Ours(){
        delete [] buckets;
        delete BF;
    }

    bool CheckBF(const uint64_t item){
        bool ret = true;
        for(int i = 0;i < K;++i){
            if(!BF->SetNGet(hash(item, i + 101) % BF_length)){
                ret = false;
            }
        }
        return ret;
    }

    void Insert(uint64_t item, uint32_t src, uint32_t dst){
        uint32_t pos = hash(src) % length;

        for(uint32_t i = 0;i < SLOT_NUM;++i){
            if(buckets[pos].items[i] == src){
                buckets[pos].counters[i] += (!CheckBF(item));
                return;
            }
        }

        uint32_t minPos = 0;
        int32_t minValue = buckets[pos].counters[0];

        for(uint32_t i = 0;i < SLOT_NUM;++i){
            if(buckets[pos].counters[i] < minValue){
                minPos = i;
                minValue = buckets[pos].counters[i];
            }
        }

        if(hash(item, 1001) < (double)0x100000000L / (minValue + 1)){
            if(!CheckBF(item)){
                buckets[pos].items[minPos] = src;
                buckets[pos].counters[minPos] += 1;
            }
        }
    }

    int32_t Query(const uint32_t src){
        return buckets[hash(src) % length].Query(src);
    }

    uint32_t Report(uint32_t HIT){
	uint32_t ret = 0;

        for(uint32_t i = 0;i < length;++i){
	    for(uint32_t j = 0;j < SLOT_NUM;++j){
            	if(buckets[i].counters[j] > HIT){
               		ret += 1;
            	}
	    }
        }

        return ret;
    }

private:
    const uint32_t K;

    uint32_t length;
    uint32_t BF_length;

    Bucket* buckets;
    BitSet* BF;
};

#endif
