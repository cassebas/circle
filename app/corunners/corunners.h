//
// corunners.h
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
#ifndef _corunners_h
#define _corunners_h

#include "corunners_definition.h"

#include <circle/multicore.h>
#include <circle/screen.h>
#include <circle/memory.h>
#include <circle/types.h>
#include <circle/logger.h>

extern "C" {
	#include "armv8_pm.h"
	#include "synthetic_bench.h"
	#include "malardalen.h"
	#include "sdvbs/disparity/disparity.h"
}

class CoRunners : public CMultiCoreSupport
{
public:
	CoRunners (CScreenDevice *pScreen, CMemorySystem *pMemorySystem);
	~CoRunners (void);
	void Run (unsigned nCore);

private:
	void RunCore0 (void);
	void RunCore1 (void);
	void RunCore2 (void);
	void RunCore3 (void);
	void SyncMaster(CSpinLock&);
	void SyncSlave(CSpinLock&, unsigned);

private:
	CScreenDevice *m_pScreen;
	CLogger* m_log;
	CSpinLock m_SyncLock;
	unsigned m_CoreWaiting;
#if defined BENCH_CONFIG_CORE0_1_1 || defined BENCH_CONFIG_CORE0_1_2
	volatile bigstruct_t* mydata1;
#else
	#if defined BENCH_CONFIG_CORE0_1_3 || defined BENCH_CONFIG_CORE0_1_4
	volatile bigstruct_t* mydata1;
	volatile int* myrandidx1;
	#endif
#endif
#if defined BENCH_CONFIG_CORE1_1_1 || defined BENCH_CONFIG_CORE1_1_2
	volatile bigstruct_t* mydata2;
#else
	#if defined BENCH_CONFIG_CORE1_1_3 || defined BENCH_CONFIG_CORE1_1_4
	volatile bigstruct_t* mydata2;
	volatile int* myrandidx2;
	#endif
#endif
#if defined BENCH_CONFIG_CORE2_1_1 || defined BENCH_CONFIG_CORE2_1_2
	volatile bigstruct_t* mydata3;
#else
	#if defined BENCH_CONFIG_CORE2_1_3 || defined BENCH_CONFIG_CORE2_1_4
	volatile bigstruct_t* mydata3;
	volatile int* myrandidx3;
	#endif
#endif
#if defined BENCH_CONFIG_CORE3_1_1 || defined BENCH_CONFIG_CORE3_1_2
	volatile bigstruct_t* mydata4;
#else
	#if defined BENCH_CONFIG_CORE3_1_3 || defined BENCH_CONFIG_CORE3_1_4
	volatile bigstruct_t* mydata4;
	volatile int* myrandidx4;
	#endif
#endif
#ifdef BENCH_CONFIG_CORE0_2_1
	volatile int* Array1;
#endif
#ifdef BENCH_CONFIG_CORE1_2_1
	volatile int* Array2;
#endif
#ifdef BENCH_CONFIG_CORE2_2_1
	volatile int* Array3;
#endif
#ifdef BENCH_CONFIG_CORE3_2_1
	volatile int* Array4;
#endif
#ifdef BENCH_CONFIG_CORE0_2_3
	matrix matA1;
	matrix matB1;
	matrix matC1;
#endif
#ifdef BENCH_CONFIG_CORE1_2_3
	matrix matA2;
	matrix matB2;
	matrix matC2;
#endif
#ifdef BENCH_CONFIG_CORE2_2_3
	matrix matA3;
	matrix matB3;
	matrix matC3;
#endif
#ifdef BENCH_CONFIG_CORE3_2_3
	matrix matA4;
	matrix matB4;
	matrix matC4;
#endif
};

#endif // _corunners_h
