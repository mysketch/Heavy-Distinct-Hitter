#ifndef DWS_H
#define DWS_H

#include "Abstract.h"

class DWS : public Abstract{
public:

#define COUNTNUM 16

    struct dCounter{
        uint32_t heapPos;
        double seed;
        double estimation;
        double count[COUNTNUM];
    };

    typedef std::unordered_map<uint32_t, dCounter> SrcMap;
    typedef std::pair<double, uint32_t> KV;

    DWS(uint64_t memory){
        MAX_SIZE = memory / (sizeof(double) + sizeof(uint32_t) +
                2 * (sizeof(uint32_t) + sizeof(dCounter)));
        MEMORY = memory;
        NAME = "DWS";
        threshold = 1;
    }

    ~DWS(){
    }

    void Insert(uint64_t item, uint32_t src, uint32_t dst){
        double seed = hash(item, 1001) / (double)0x100000000L;

        if(mp.find(src) != mp.end()){
            uint32_t pos = hash(item) % COUNTNUM;
            if(seed < mp[src].count[pos]){
                double sum = 0;
                for(uint32_t i = 0;i < COUNTNUM;++i){
                    sum += mp[src].count[i];
                }
                mp[src].estimation += (COUNTNUM / sum);

                mp[src].count[pos] = seed;
                mp[src].seed = MIN(mp[src].seed, seed);

                heap[mp[src].heapPos].first = mp[src].seed;
                heap_down(mp[src].heapPos);
            }
        }
        else{
            if(seed < threshold){
                mp[src].seed = seed;
                mp[src].estimation = 0;

                for(uint32_t i = 0;i < COUNTNUM;++i){
                    mp[src].count[i] = 1;
                }

                uint32_t pos = hash(item) % COUNTNUM;
                mp[src].count[pos] = seed;

                if(heap.size() < MAX_SIZE){
                    heap.push_back(KV(seed, src));

                    mp[src].heapPos = heap.size() - 1;
                    heap_up(heap.size() - 1);
                }
                else{
                    KV &kv = heap[0];

                    threshold = mp[kv.second].seed;

                    mp.erase(kv.second);
                    kv.second = src;
                    kv.first = seed;

                    mp[src].heapPos = 0;
                    heap_down(0);
                }
            }
        }
    }

    int32_t Query(const uint32_t src){
        if(mp.find(src) == mp.end())
            return 0;
        return mp[src].estimation;
    }

    uint32_t Report(uint32_t HIT){
	uint32_t ret = 0;

	for(auto it = mp.begin();it != mp.end();++it){
	    if(it->second.estimation > HIT){
		ret += 1;
	    }
	}

	return ret;
    }

private:
    uint32_t MAX_SIZE;
    double threshold;

    SrcMap mp;
    std::vector<KV> heap;


    void heap_down(int32_t i){
        while (i < heap.size() / 2) {
            int32_t l_child = 2 * i + 1;
            int32_t r_child = 2 * i + 2;
            int32_t larger_one = i;
            if (l_child < heap.size() && heap[l_child] > heap[larger_one]) {
                larger_one = l_child;
            }
            if (r_child < heap.size() && heap[r_child] > heap[larger_one]) {
                larger_one = r_child;
            }
            if (larger_one != i) {
                std::swap(heap[i], heap[larger_one]);
                std::swap(mp[heap[i].second].heapPos, mp[heap[larger_one].second].heapPos);
                heap_down(larger_one);
            } else {
                break;
            }
        }
    }

    void heap_up(int32_t i) {
        while (i > 1) {
            int32_t parent = (i - 1) / 2;
            if (heap[parent] >= heap[i]) {
                break;
            }
            std::swap(heap[i], heap[parent]);
            std::swap(mp[heap[i].second].heapPos, mp[heap[parent].second].heapPos);
            i = parent;
        }
    }
};

#endif 
