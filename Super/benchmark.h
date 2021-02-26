#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <sys/stat.h>

#include <chrono>

#include "DWS.h"
#include "Ours.h"
#include "Spread.h"

#include "common/MMap.h"

class BenchMark{
public:

    typedef std::vector<Abstract*> AbsVector;
    typedef std::unordered_map<uint32_t, int32_t> HashMap;

    BenchMark(const char* _PATH):
            PATH(_PATH){
        TOTAL = 0;
        result = Load(PATH);

        start = (uint64_t *)result.start;
        SIZE = result.length / sizeof(uint64_t);

        std::unordered_set<uint64_t> st;

        for(uint32_t i = 0;i < SIZE;++i){
            if(st.find(start[i]) == st.end()){
                st.insert(start[i]);
                mp[start[i]] += 1;
                TOTAL += 1;
            }
        }
    }

    void TopKError(double alpha){
        AbsVector FPIs = {
            new Ours<8>(300000, 4, 0.25),
            new Ours<8>(350000, 4, 0.25),
            new Ours<8>(400000, 4, 0.25),
            new Ours<8>(450000, 4, 0.25),
            new Ours<8>(500000, 4, 0.25),
        };

        BenchInsert(FPIs);

        for(auto FPI : FPIs){
            std::cout << "Check," << FPI->NAME << "," << FPI->MEMORY << std::endl;
            FPICheckError(FPI, alpha * TOTAL);
            delete FPI;
        }
    }

private:
    LoadResult result;
    uint64_t* start;

    uint64_t SIZE;
    double TOTAL;

    HashMap mp;
    const char* PATH;

    std::vector<std::vector<double>> table;
    std::unordered_map<uint32_t, uint32_t> paraMp;
    std::unordered_map<std::string, uint32_t> nameMp;

    typedef std::chrono::high_resolution_clock::time_point TP;

    inline TP now(){
        return std::chrono::high_resolution_clock::now();
    }

    inline double durationms(TP finish, TP start){
        return std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000>>>(finish - start).count();
    }

    void BenchInsert(AbsVector& sketches){
        for(uint32_t i = 0;i < SIZE;++i){
            for(auto sketch : sketches)
                sketch->Insert(start[i], start[i], start[i] >> 32);
        }
    }

    void InsertThp(Abstract* sketch){
        TP initial, finish;

        initial = now();
        for(uint32_t i = 0;i < SIZE;++i){
            sketch->Insert(start[i], start[i], start[i] >> 32);
        }
        finish = now();

	    std::cout << "Insertion Thp: " << SIZE / durationms(finish, initial) << std::endl;
    }

    void ReportThp(Abstract* sketch, uint32_t HIT){
        TP initial, finish;
	uint32_t result;

        initial = now();
        result = sketch->Report(HIT);
        finish = now();

       	std::cout << "Report Thp: " << result << ", " << durationms(finish, initial) << std::endl;
    }


    void FPICheckError(Abstract* sketch, uint32_t HIT){
        double real = 0, estimate = 0, both = 0;
        double aae = 0, are = 0, cr = 0, pr = 0, f1 = 0, fnr = 0, fpr = 0;

        double hot = 0;

        for(auto it = mp.begin();it != mp.end();++it){
            int32_t value = sketch->Query(it->first);

            if(value > HIT){
                estimate += 1;
                if(it->second > HIT) {
                    both += 1;
                    aae += abs(it->second - value);
                    are += abs(it->second - value) / (double)it->second;
                }
            }
            if(it->second > HIT){
                hot += it->second;
                real += 1;
            }
        }

        if(both <= 0){
            std::cout << "Not Find Real Super" << std::endl;
        }
        else{
            aae /= both;
            are /= both;
        }

        cr = both / real;

        if(estimate <= 0){
            std::cout << "Not Find Super" << std::endl;
        }
        else{
            pr = both / estimate;
        }

        fnr = (real - both) / real;
        fpr = (estimate - both) / (mp.size() - real);

	    std::cout << "AAE," << aae << std::endl
		    << "ARE," << are << std::endl
		    << "CR," << cr << std::endl
		    << "PR," << pr << std::endl
            << "FNR," << fnr << std::endl
            << "FPR," << fpr << std::endl;
    }

};

#endif 
