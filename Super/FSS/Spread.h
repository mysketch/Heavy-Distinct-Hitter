#ifndef SPREAD_H
#define SPREAD_H

#include "bitset.h"
#include "Abstract.h"
#include <immintrin.h>

class Spread : public Abstract{
public:
    struct FanOut{
        uint64_t bits[6];

        uint32_t Estimate(){
            double ret = 0;
            uint32_t base, zeros;

            for(base = 0;base < 4;++base){
                zeros = 64 - _mm_popcnt_u64(bits[base]);
                if(zeros > 5){
                    break;
                }
            }

            if(base < 4){
                for(uint32_t i = base;i < 4;++i){
                    zeros = 64 - _mm_popcnt_u64(bits[i]);
                    ret += 64.0 * log(64.0 / zeros);
                }
            }

            zeros = 128 - _mm_popcnt_u64(bits[4]) - _mm_popcnt_u64(bits[5]);
            ret += 128.0 * log(128.0 / zeros);

            return ret * (1 << base);
        }

        void Insert(uint32_t hResult, uint32_t level){
            if(level < 4){
                uint32_t pos = hResult % 64;
                Set((uint8_t*)&bits[level], pos);
            }
            else{
                uint32_t pos = hResult % 128;
                Set((uint8_t*)&bits[4], pos);
            }
        }
    };

    struct Bucket{
        uint32_t level;
        uint32_t item;
        FanOut fanout;
    };

    Spread(uint64_t memory, uint32_t _HASH_NUM):
        HASH_NUM(_HASH_NUM){
        MEMORY = memory;
        NAME = "Spread";

        LENGTH = memory / HASH_NUM / sizeof(Bucket);
        buckets = new Bucket* [HASH_NUM];
        for(uint32_t i = 0;i < HASH_NUM;++i){
            buckets[i] = new Bucket [LENGTH];
            memset(buckets[i], 0, LENGTH * sizeof(Bucket));
        }
    }

    ~Spread(){
        for(uint32_t i = 0;i < HASH_NUM;++i){
            delete [] buckets[i];
        }
        delete [] buckets;
    }

    void Insert(uint64_t item, uint32_t src, uint32_t dst){
        uint32_t hResult = hash(item, 1001);
        uint32_t level = __builtin_clz(hResult);

        for(uint32_t i = 0;i < HASH_NUM;++i){
            uint32_t pos = hash(src, i) % LENGTH;
            if(buckets[i][pos].level <= level){
                buckets[i][pos].level = level;
                buckets[i][pos].item = src;
            }
            buckets[i][pos].fanout.Insert(hResult, level);
        }
    }

    int32_t Query(const uint32_t src){
        uint32_t find = false, minimum = UINT32_MAX;

        for(uint32_t i = 0;i < HASH_NUM;++i){
            uint32_t pos = hash(src, i) % LENGTH;
            if(buckets[i][pos].item == src){
                find = true;
                break;
            }
        }

        if(!find)
            return 0;

        for(uint32_t i = 0;i < HASH_NUM;++i){
            uint32_t pos = hash(src, i) % LENGTH;
            minimum = MIN(minimum, buckets[i][pos].fanout.Estimate());
        }

        return minimum;
    }

    uint32_t Report(uint32_t HIT){
	uint32_t ret = 0;

        for(uint32_t i = 0;i < HASH_NUM;++i){
            for(uint32_t j = 0;j < LENGTH;++j){
		uint32_t minimum = buckets[i][j].fanout.Estimate();
		uint32_t src = buckets[i][j].item;
                if(minimum > HIT){
		    for(uint32_t k = 0;k < HASH_NUM;++k){
            		uint32_t pos = hash(src, k) % LENGTH;
            		minimum = MIN(minimum, buckets[i][pos].fanout.Estimate());
        	    }

		    if(minimum > HIT)
                	ret += 1;
		}
            }
        }

        return ret;
    }

private:
    uint32_t LENGTH;
    uint32_t HASH_NUM;

    Bucket** buckets;
};

#endif 
