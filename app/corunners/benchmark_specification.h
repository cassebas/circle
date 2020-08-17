#ifndef BENCHMARK_SPECIFICATION_H
#define BENCHMARK_SPECIFICATION_H


/*
 *  ********************
 *  SYNTHETIC BENCHMARKS
 *  ********************
 */

// SYNTHETIC BENCH LINEAR ARRAY READ/WRITE
#define BENCH_SYNBENCH_LINEARARRAY_DECL(corenum)	\
	volatile bigstruct_t* mydata##corenum;

#define BENCH_SYNBENCH_LINEARARRAY_INIT1(corenum)			\
	mydata##corenum = new bigstruct_t[SYNBENCH_DATASIZE];

// SYNTHETIC BENCH RANDOM ARRAY READ/WRITE
#define BENCH_SYNBENCH_RANDOMARRAY_DECL(corenum)	\
	volatile bigstruct_t* mydata##corenum;			\
	volatile int* myrandidx##corenum;

#define BENCH_SYNBENCH_RANDOMARRAY_INIT1(corenum)			\
	mydata##corenum = new bigstruct_t[SYNBENCH_DATASIZE];	\
	myrandidx##corenum = new int[SYNBENCH_DATASIZE];

#define BENCH_SYNBENCH_RANDOMARRAY_INIT2(corenum)		\
	array_access_randomize(myrandidx##corenum, &rand);


/*
 *  **********
 *  MALARDALEN
 *  **********
 */

// MALARDALEN BSORT
#define BENCH_MALARDALEN_BSORT_DECL(corenum)	\
	volatile int* Array##corenum;

#define BENCH_MALARDALEN_BSORT_INIT1(corenum)						\
	BENCH_ARG_CORE##corenum = (volatile int*) new int[NUMELEMS];

#define BENCH_MALARDALEN_BSORT_INIT2(corenum)				\
	bsort100_Initialize(BENCH_ARG_CORE##corenum, &rand);

// MALARDALEN MATMULT
#define BENCH_MALARDALEN_MATMULT_DECL(corenum)	\
	matrix matA##corenum;						\
	matrix matB##corenum;						\
	matrix matC##corenum;

#define BENCH_MALARDALEN_MATMULT_INIT2(corenum)	\
	matmult_Initialize(matA##corenum);			\
	matmult_Initialize(matB##corenum);


/*
 *  *****************************************
 *  SAN DIEGO VISUAL BENCHMARK SUITE (SD-VBS)
 *  *****************************************
 */
#define BENCH_SDVBS_DISPARITY_INIT1							\
	int width=DISPARITY_INPUTSIZE, height=DISPARITY_INPUTSIZE;	\
    int WIN_SZ=4, SHIFT=8;										\
    I2D* srcImage1 = iMallocHandle(width, height);				\
    I2D* srcImage2 = iMallocHandle(width, height);

#define BENCH_SDVBS_DISPARITY_INIT2							\
	for (int i=0; i<(width*height); i++) { \
		srcImage1->data[i] = rand.GetNumber() % 256; \
		srcImage2->data[i] = rand.GetNumber() % 256; \
	}

#endif // BENCHMARK_SPECIFICATION_H
