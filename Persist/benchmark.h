#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>

#include "SS.h"
#include "OnOff.h"
#include "Ours.h"

#include "common/MMap.h"

template<typename DATA_TYPE,typename COUNT_TYPE>
class BenchMark{
public:

    typedef std::vector<Abstract<DATA_TYPE, COUNT_TYPE>*> AbsVector;
    typedef std::unordered_map<DATA_TYPE, COUNT_TYPE> HashMap;

    BenchMark(const char* _PATH, const COUNT_TYPE _T):
            PATH(_PATH){
        result = Load(PATH);
        start = (DATA_TYPE*)result.start;
        SIZE = result.length / sizeof(DATA_TYPE);

	    LENGTH = SIZE / (double)(_T);

        COUNT_TYPE number = 0;
        HashMap record;

        TOTAL = 0;
        T = 0;

        for(uint32_t i = 0;i < SIZE;++i){
            if(number % LENGTH == 0)
                T += 1;
            number += 1;

            if(record[start[i]] != T){
                record[start[i]] = T;
                mp[start[i]] += 1;
                TOTAL += 1;
            }
        }
    }

    ~BenchMark(){
        UnLoad(result);
    }

    void TopKError(double alpha){
        AbsVector FPIs = {
            new Ours<DATA_TYPE, COUNT_TYPE, 8>(50000, 4),
            new Ours<DATA_TYPE, COUNT_TYPE, 8>(75000, 4),
            new Ours<DATA_TYPE, COUNT_TYPE, 8>(100000, 4),
            new Ours<DATA_TYPE, COUNT_TYPE, 8>(125000, 4),
            new Ours<DATA_TYPE, COUNT_TYPE, 8>(150000, 4),
	};

        BenchInsert(FPIs);

        for(auto FPI : FPIs){
            FPICheckError(FPI, alpha * TOTAL);
            delete FPI;
        }
    }

private:
    LoadResult result;
    DATA_TYPE* start;

    uint64_t SIZE;

    double TOTAL;
    COUNT_TYPE T;
    COUNT_TYPE LENGTH;

    HashMap mp;
    const char* PATH;

    typedef std::chrono::high_resolution_clock::time_point TP;

    inline TP now(){
        return std::chrono::high_resolution_clock::now();
    }

    inline double durationms(TP finish, TP start){
        return std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000>>>(finish - start).count();
    }

    typedef typename Abstract<DATA_TYPE, COUNT_TYPE>::Pair Pair;

    void BenchInsert(AbsVector sketches){
        COUNT_TYPE number = 0, windowId = 0;
        Pair pair;

        for(uint32_t i = 0;i < SIZE;++i){
            if(number % LENGTH == 0){
                windowId += 1;
                for(auto sketch : sketches)
                    sketch->NewWindow(windowId);
            }
            number += 1;
            pair.Set(start[i], windowId);

            for(auto sketch : sketches)
                sketch->Insert(pair, start[i], windowId);
        }
    }

    void InsertThp(Abstract<DATA_TYPE, COUNT_TYPE>* sketch){
        TP initial, finish;

        COUNT_TYPE number = 0, windowId = 0;
        Pair pair;

        initial = now();
        for(uint32_t i = 0;i < SIZE;++i){
            if(number % LENGTH == 0){
                windowId += 1;
                sketch->NewWindow(windowId);
            }
            number += 1;
            pair.Set(start[i], windowId);

            sketch->Insert(pair, start[i], windowId);
        }
        finish = now();

	    std::cout << "Thp: " << number / durationms(finish, initial) << std::endl;
    }

    void FPICheckError(Abstract<DATA_TYPE, COUNT_TYPE>* sketch, COUNT_TYPE HIT){
        double real = 0, estimate = 0, both = 0;
        double aae = 0, are = 0, cr = 0, pr = 0, f1 = 0, fnr = 0, fpr = 0;

        for(auto it = mp.begin();it != mp.end();++it){
            COUNT_TYPE value = sketch->Query(it->first);

            if(value > HIT){
                estimate += 1;
                if(it->second > HIT) {
                    both += 1;
                    aae += abs(it->second - value);
                    are += abs(it->second - value) / (double)it->second;
                }
            }
            if(it->second > HIT)
                real += 1;
        }

        if(both <= 0){
            std::cout << "Not Find Real Persistent" << std::endl;
        }
        else{
            aae /= both;
            are /= both;
        }

        cr = both / real;

        if(estimate <= 0){
            std::cout << "Not Find Persistent" << std::endl;
        }
        else{
            pr = both / estimate;
        }

        fnr = (real - both) / real;
        fpr = (estimate - both) / (mp.size() - real);

        if(cr == 0 && pr == 0)
            f1 = 0;
        else
            f1 = (2*cr*pr)/(cr+pr);

	    std::cout << "AAE: " << aae << std::endl
		    << "ARE: " << are << std::endl
		    << "CR: " << cr << std::endl
		    << "PR: " << pr << std::endl
		    << "FNR: " << fnr << std::endl
		    << "FPR: " << fpr << std::endl << std::endl;
    }

};

#endif 
