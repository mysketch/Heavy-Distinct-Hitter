#ifndef OURS_H
#define OURS_H

#include "bitset.h"
#include "Abstract.h"

template<typename DATA_TYPE, typename COUNT_TYPE, uint32_t SLOT_NUM>
class Ours : public Abstract<DATA_TYPE, COUNT_TYPE> {
public:

    struct Bucket {
        DATA_TYPE items[SLOT_NUM];
        COUNT_TYPE counters[SLOT_NUM];

        inline COUNT_TYPE Query(const DATA_TYPE item){
            for(uint32_t i = 0;i < SLOT_NUM;++i){
                if(items[i] == item)
                    return counters[i];
            }
            return 0;
        }
    };

    Ours(uint64_t memory, uint32_t _K) : K(_K),
            length(memory * 0.9 / sizeof(Bucket)){
        BF_length = memory * 0.1 * 8;
        BF = new BitSet(BF_length);
        buckets = new Bucket[length];
        memset(buckets, 0, length * sizeof(Bucket));
        //std::cout << "Memory: " << BF_length / 8 + length * sizeof(Bucket) << std::endl;
    }

    ~Ours(){
        delete [] buckets;
        delete BF;
    }

    bool CheckBF(const DATA_TYPE item){
        bool ret = true;
        for(int i = 0;i < K;++i){
            if(!BF->SetNGet(this->hash(item, i + 101) % BF_length)){
                ret = false;
            }
        }
        return ret;
    }

    typedef typename Abstract<DATA_TYPE, COUNT_TYPE>::Pair Pair;

    void Insert(Pair& pair, const DATA_TYPE item, const COUNT_TYPE window){
        uint32_t pos = this->hash(item) % length;

        for(uint32_t i = 0;i < SLOT_NUM;++i){
            if(buckets[pos].items[i] == item){
                buckets[pos].counters[i] += (!CheckBF(item));
                return;
            }
        }

        uint32_t minPos = 0;
        COUNT_TYPE minValue = buckets[pos].counters[0];

        for(uint32_t i = 1;i < SLOT_NUM;++i){
            if(buckets[pos].counters[i] < minValue){
                minPos = i;
                minValue = buckets[pos].counters[i];
            }
        }

        if(pair.hash() < (double)0x100000000L / (minValue + 1)){
            if(!CheckBF(item)){
                buckets[pos].items[minPos] = item;
                buckets[pos].counters[minPos] += 1;
            }
        }
    }

    COUNT_TYPE Query(const DATA_TYPE item){
        return buckets[this->hash(item) % length].Query(item);
    }

    void NewWindow(const COUNT_TYPE window){
        BF->Clear();
    }

private:
    const uint32_t K;
    const uint32_t length;

    uint32_t BF_length;

    Bucket* buckets;
    BitSet* BF;
};

#endif 
