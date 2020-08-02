/**
 * synthetic_bench.h
 *
 *  Created on: Nov 11, 2019
 *      Author: Caspar Treijtel
 */
#ifndef SYNTHETIC_BENCH_H
#define SYNTHETIC_BENCH_H

/**
 * We want to use a data structure that consists of more than
 * 512KB (size of L2 cache). We'll use 10,240 arrays of 64 bytes
 * large. The 64 bytes is chosen because we want to touch as
 * many cache lines as possible, with a jump of 64 bytes each
 * iteration, we try to fill the L2 as soon as possible.
 * This is because the cache (both L1 and L2) have cache line
 * sizes of 64 bytes.
 *
 * The default number of arrays can be altered (on the m4 cmdline),
 * in order to see the effects of the stressing the memory hierarchy
 * with increasing data structure sizes.
 */

/* Maybe define the size of the array that is used in the
   synthetic benchmarks.
   If already defined in the CFLAGS (-DSYNBENCH_DATASIZE=...) then
   do nothing. */
#ifndef SYNBENCH_DATASIZE
#define SYNBENCH_DATASIZE 655360
#endif
// To make bigstruct 64 bytes, which is equal to the line size of the caches
// id + data == 64 bytes
#define BIGSTRUCT_DATASIZE 63

/**
 * Synthetic benchmark inspired by the paper `Predictable and Efficient Virtual
 * Addressing for Safety-Critical Real-Time Systems', written by Bennet and
 * Audsley (2001).
 */

/**
 * Type bigstruct_t is (with int being 4 bytes) a 256 bytes struct.
 */
typedef struct bigstruct {
	int id;
	int data[BIGSTRUCT_DATASIZE];
} bigstruct_t;


int array_access_linear(volatile bigstruct_t* data);
void array_write_linear(volatile bigstruct_t* data);
void array_access_randomize(volatile int* idx, int corenum);
int array_access_random(volatile bigstruct_t* data, volatile int* idx);
void array_write_random(volatile bigstruct_t* data, volatile int* idx);
void array_access_alternate(volatile bigstruct_t* data);

#endif /* SYNTHETIC_BENCH_H */
