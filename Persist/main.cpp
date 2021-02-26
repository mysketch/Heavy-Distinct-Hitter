#include "benchmark.h"

int main(int argc, char *argv[]) {
    for(uint32_t i = 1;i < argc;++i){
	    std::cout << argv[i] << std::endl;
        BenchMark<uint32_t, int32_t> dataset(argv[i], 1600);
        dataset.TopKError(0.00005);
    }
    return 0;
}
