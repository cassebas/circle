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
 *   Core 1: Mälardalen edn
 *   Core 2: Synthetic benchmark random array access
 */
#define NR_OF_CORES 3
#define BENCH_CONFIG_CORE0_3_1
#define BENCH_CONFIG_CORE1_2_2
#define BENCH_CONFIG_CORE2_1_3

#define CONFIG_SERIES_STRING "config_series: '321'"
#define CONFIG_BENCH_STRING "config_benchmarks: '123'"

#define BENCH_STRING_CORE0 "benchmark: sdvbs_disparity"
#define BENCH_STRING_CORE1 "benchmark: malardalen_edn"
#define BENCH_STRING_CORE2 "benchmark: linear_array_write"
#define BENCH_STRING_CORE3 ""

#define BENCH_ARG_CORE0 Array1
#define BENCH_ARG_CORE2 mydata3

#define DO_BENCH_CORE0 getDisparity(srcImage1, srcImage2, WIN_SZ, SHIFT);
#define DO_BENCH_CORE1 edn_Calculate();
#define DO_BENCH_CORE2 array_write_linear(BENCH_ARG_CORE2);
#define DO_BENCH_CORE3

#define EXP_LABEL "DEFAULT"

/**
 * Specific configuration for the Mälardalen bsort benchmark
 */
#define BSORT_INPUTSIZE 100

/**
 * Specific configuration for the Mälardalen bsort benchmark
 */
#define MATMULT_INPUTSIZE 100

/**
 * Specific configuration for the SD-VBS Disparity benchmark
 */
#define DISPARITY_INPUTSIZE 96

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


#endif // corunners_defintion_h

