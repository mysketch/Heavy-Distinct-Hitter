DistinctSketch
============

Introduction
--------
The DistinctSketch is an algorithm that addresses finding heavy distinct hitters. It can work well on two important applications in finding heavy distinct hitters: finding super-spreaders and finding persistent elements. The key technique of the DistinctSketch is the Early Dropping. Our Early Dropping can make irrelevant data dropped before storage by designing a mechanism of replacement. Without storing irrelevant data, the memory consumption of the DistinctSketch is significantly reduced. Further, we optimize the data structure by Bucketized Compacting to achieve both memory efficiency and high throughput. We provide the analysis of the error bound and conduct extensive experiments on two applications. In finding super-spreaders, our experimental results show that our DistinctSketch achieves 162 times lower error on average than the state-of-the-art sketch-based algorithm. In finding persistent elements, our experimental results show that our DistinctSketch achieves 44 times lower error on average than the state-of-the-art sketch-based algorithm.

Repository structure
--------------------
*  `Super/`: benchmark and algorithms of finding super-spreaders
*  `Persist/`: benchmark and algorithms of finding persistent elements

Requirements
-------
- cmake
- g++

We conducted all experiments on a machine with one 12-core processor (24 threads, Intel(R) Xeon(R) CPU E5-2620 CPU @ 2.00GHz) and 128 GB DRAM memory. Each processor has three levels of cache memory: one 32KB L1 data cache and one 32KB L1 instruction cache for each core, one 256KB L2 cache for each core, and one 15MB L3 cache shared by all cores.

How to run
-------

For finding super-spreaders,

```bash
$ cd ./Super
$ cmake .
$ make
$ ./Super your-dataset
```
