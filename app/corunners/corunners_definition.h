//
// corunners_definition.h
//
// CoRunners application running on the Circle platform
// Copyright (C) 2020  Cassebas <cassebas@cosmico.nl>
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2015-2016  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef corunners_definition_h
#define corunners_definition_h

/**
 * Extension of corunners app with machinery to perform benchmark
 * evaluations.
 *
 * Default configuration is 3 cores, running:
 *   Core 0: SD-VBS disparity
 *   Core 1: Mälardalen bsort
 *   Core 2: Synthetic benchmark linear array write
 */
#define NR_OF_CORES 3
#define BENCH_CONFIG_CORE0_3_1
#define BENCH_CONFIG_CORE1_2_1
#define BENCH_CONFIG_CORE2_1_2

#define CONFIG_SERIES_STRING "config_series: '321'"
#define CONFIG_BENCH_STRING "config_benchmarks: '112'"

#define BENCH_STRING_CORE0 "benchmark: sdvbs_disparity"
#define BENCH_STRING_CORE1 "benchmark: malardalen_bsort"
#define BENCH_STRING_CORE2 "benchmark: linear_array_write"
#define BENCH_STRING_CORE3 ""

#define BENCH_DECL_CORE0
#define BENCH_DECL_CORE1 volatile int* Array1;
#define BENCH_DECL_CORE2 volatile bigstruct_t* mydata2;
#define BENCH_DECL_CORE3

#define BENCH_INIT1_CORE0 \
	int width=SDVBS_DISPARITY_INPUTSIZE, height=SDVBS_DISPARITY_INPUTSIZE;	\
    int WIN_SZ=4, SHIFT=8;										\
    I2D* srcImage1 = iMallocHandle(width, height);				\
    I2D* srcImage2 = iMallocHandle(width, height);
#define BENCH_INIT1_CORE1 Array1 = (volatile int*) new int[MALARDALEN_BSORT_INPUTSIZE];
#define BENCH_INIT1_CORE2 mydata2 = new bigstruct_t[SYNBENCH_DATASIZE];
#define BENCH_INIT1_CORE3

#define BENCH_INIT2_CORE0 \
	for (int i=0; i<(width*height); i++) { \
		srcImage1->data[i] = rand.GetNumber() % 256; \
		srcImage2->data[i] = rand.GetNumber() % 256; \
	}
#define BENCH_INIT2_CORE1 bsort100_Initialize(Array1, &rand);
#define BENCH_INIT2_CORE2
#define BENCH_INIT2_CORE3

#define DO_BENCH_CORE0 getDisparity(srcImage1, srcImage2, WIN_SZ, SHIFT);
#define DO_BENCH_CORE1 bsort100_BubbleSort(Array1);
#define DO_BENCH_CORE2 array_write_linear(mydata2);
#define DO_BENCH_CORE3

#define BENCH_CLEANUP_CORE0
#define BENCH_CLEANUP_CORE1 free((void*) Array1);
#define BENCH_CLEANUP_CORE2 free((void*) mydata2);
#define BENCH_CLEANUP_CORE3

#define EXP_LABEL "DEFAULT"

/**
 * Input size configuration for each core: default for disparity 64
 */
#define INPUTSIZE_CORE0 64

/**
 * Input size configuration for each core: default for bsort 100
 */
#define INPUTSIZE_CORE1 100

/**
 * Input size configuration for each core: default for array write linear: 131072
 */
#define INPUTSIZE_CORE2 131072

#define SDVBS_DISPARITY_INPUTSIZE INPUTSIZE_CORE0
#define MALARDALEN_BSORT_INPUTSIZE INPUTSIZE_CORE1
#define SYNBENCH_DATASIZE INPUTSIZE_CORE2

/**
 * If the following macro with name BENCHMARK_CONFIG_M4 was
 * specified on the command line, then we know the include
 * file can be included.
 * This is done to make sure that the include file is actually
 * generated (which is done using m4).
 *
 * Usage is thus:
 *   m4 config='123' benchmark_config.m4 > benchmark_config.h && \
 *     make BENCHMARK_CONFIG=-DBENCHMARK_CONFIG_M4 Pi-64
 *
 * Or, just 
 *   make Pi-64 in case the benchmark config is not to be set.
 */
#ifdef BENCHMARK_CONFIG_M4
#include "benchmark_config.h"
#endif

/**
 * Optionally include the benchmark_specification.h header file,
 * which contains declaration and initialization code, as an 
 * alternative to the generated benchmark_config.h header file
 * which is generated by benchmark_config.m4.
 */
/* #include "benchmark_specification.h" */

#endif // corunners_defintion_h

