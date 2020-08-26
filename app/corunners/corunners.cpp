//
// corunners.cpp
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
#include "corunners.h"
#include <stddef.h>
#include <circle/sched/scheduler.h>
#include <circle/alloc.h>
#include <circle/new.h>
#include <circle/spinlock.h>
#include <circle/types.h>
#include <circle/synchronize64.h>

#include "randomwrapper.h"

// These are methods that are implemented in assembly
// See ldstexcl.S
extern "C" {
	int write_bit_atomic(unsigned, boolean, unsigned*);
	boolean read_bit_atomic(unsigned, unsigned*);
	unsigned read_atomic(unsigned* ptr);
	volatile void countdown(u64);
}


/**
 * Simple function that makes the processor perform a busy
 * wait by counting down from a specific number. The number
 * of cycles spent each time is measured and printed.
 */
static inline u64 report_cycles_countdown(u64 count)
{
    u64 cycles;

	enable_cycle_counter();
	reset_cycle_counter();
	//////////////////////
	// now wait a while //
	//////////////////////
	countdown(count);
	///////////////////
	disable_cycle_counter();
	cycles = read_cycle_counter();
	return cycles;
}


#ifdef DisableInterrupts
#undef DisableInterrupts
#endif
static inline void DisableInterrupts() {
	DisableIRQs();
	DisableFIQs();
}

#ifdef EnableInterrupts
#undef EnableInterrupts
#endif
static inline void EnableInterrupts() {
	EnableIRQs();
	EnableFIQs();
}

static const char FromCoRunners[] = "CoRunners";

CoRunners::CoRunners (CScreenDevice *pScreen, CMemorySystem *pMemorySystem)
:
	CMultiCoreSupport (pMemorySystem),
	m_pScreen (pScreen)
{
	m_log = CLogger::Get();
	m_CoreWaiting = 0;
}

CoRunners::~CoRunners (void)
{
	m_pScreen = 0;
	///////////////////////
	// CLEANUP VARIABLES //
	///////////////////////
	BENCH_CLEANUP_CORE0
	BENCH_CLEANUP_CORE1
	BENCH_CLEANUP_CORE2
	BENCH_CLEANUP_CORE3
	///////////////////////
}

void CoRunners::SyncMaster(CSpinLock& lock)
{
	lock.Acquire();

	// Set the flag for master, to indicate to the slaves that
	// we have set acquired the lock and will wait for the slaves
	// to start waiting
	if (write_bit_atomic(0, true, &m_CoreWaiting))
		m_log->Write(FromCoRunners, LogWarning,
					 "Core0: failure to write to memory");

	boolean exit = false;
	while (!exit) {
		exit = true;
#if NR_OF_CORES >= 2
		exit = exit && read_bit_atomic(1, &m_CoreWaiting);
#endif
#if NR_OF_CORES >= 3
		exit = exit && read_bit_atomic(2, &m_CoreWaiting);
#endif
#if NR_OF_CORES >= 4
		exit = exit && read_bit_atomic(3, &m_CoreWaiting);
#endif
	}

	for (int i=1; i<NR_OF_CORES; i++)
		if (write_bit_atomic(i, false, &m_CoreWaiting))
			m_log->Write(FromCoRunners, LogWarning,
						 "Core0: failure to write to memory");

	// Reset waiting status for master
	if (write_bit_atomic(0, false, &m_CoreWaiting))
		m_log->Write(FromCoRunners, LogWarning,
					 "Core0: failure to write to memory");

	lock.Release();
}

void CoRunners::SyncSlave(CSpinLock& lock, unsigned corenum, unsigned offset)
{
	if (write_bit_atomic(corenum, true, &m_CoreWaiting))
		m_log->Write(FromCoRunners, LogWarning,
					 "Core%d: failure to write to memory", corenum);

	// First make sure that master has acquired the lock and is
	// already waiting
	while (!read_bit_atomic(0, &m_CoreWaiting))
		;

	lock.Acquire();
	lock.Release();

	// // Maybe wait a while (create time offset in starting time co-runner)
	countdown(offset * DELAY_STEP_COUNTDOWN);
}

void CoRunners::Run (unsigned corenum)
{
	switch (corenum) {
	case 0:
		RunCore0();
		break;
	case 1:
		RunCore1();
		break;
	case 2:
		RunCore2();
		break;
	case 3:
		RunCore3();
		break;
	default:
		m_log->Write(FromCoRunners, LogError,
					 "Illegal core number %d!", corenum);
	}
}

void CoRunners::RunCore0()
{
	RandomWrapper rand;
    u64 cycles;
	unsigned corenum = 0;
    unsigned int offset=0;

	/////////////////////
	// Benchmark INIT1 //
	/////////////////////
	BENCH_INIT1_CORE0
	/////////////////////

	/* Globally enable PMU */
	enable_pmu();

#ifdef REPORT_CYCLES_COUNTDOWN
	// Before we do anything, first report the number of cycles spent
	// while busy waiting using the countdown function. This information
	// is needed to cause delays for the co-runners.
	u64 count;
	float cpc=0.0;
	for (int i=1; i<=10; i++) {
		count = i*1000000;
		cycles = report_cycles_countdown(count);
		cpc = (float) cycles / count;
		m_log->Write(FromCoRunners, LogNotice,
					 "Countdown was %lu. Cycles spent=%lu\n\r",
					 count, cycles);
		m_log->Write(FromCoRunners, LogNotice,
					 "Measured cycles per count was %.9f\n\r",
					 cpc);
	}
#endif

	unsigned iter=1;
	while (1) {
		/////////////////////
		// Benchmark INIT2 //
		/////////////////////
		BENCH_INIT2_CORE0
		/////////////////////

#ifndef DISABLE_CACHE
	#ifndef NO_CACHE_MGMT
		/* Master gets to invalidate the complete cache */
		InvalidateDataCache ();
		CleanDataCache ();
	#endif
#endif

		SyncMaster(m_SyncLock);

		enable_cycle_counter();
		reset_cycle_counter();
		///////////////////
		// DO BENCHMARK! //
		///////////////////
		DO_BENCH_CORE0
		///////////////////
		disable_cycle_counter();
		cycles = read_cycle_counter();

		m_log->Write(FromCoRunners, LogNotice,
					 "CYCLECOUNT label: %s %s %s %s cores: %d core: %d cycle_count: %12u iteration: %u offset: %d",
					 EXP_LABEL, CONFIG_SERIES_STRING, CONFIG_BENCH_STRING,
					 BENCH_STRING_CORE0, NR_OF_CORES, corenum,
					 cycles, iter, offset);

		if (++iter % ITERATIONS_PER_STEP == 0)
			offset += OFFSET_STEP_SIZE;

		// let the temperature task run (only for core 0)
		CScheduler::Get ()->Yield ();
	}
}

void CoRunners::RunCore1()
{
#if NR_OF_CORES >= 2
	RandomWrapper rand;
    u64 cycles;
	unsigned corenum = 1;
    unsigned int offset=0;

	/////////////////////
	// Benchmark INIT1 //
	/////////////////////
	BENCH_INIT1_CORE1
	/////////////////////

	/* Globally enable PMU */
	enable_pmu();

	unsigned iter=1;
	while (1) {
		//////////////////////
		// Benchmark INIT2 //
		/////////////////////
		BENCH_INIT2_CORE1
		/////////////////////

#ifndef DISABLE_CACHE
	#ifndef NO_CACHE_MGMT
		/* Slave only invalidates its own L1 cache */
		InvalidateDataCacheL1Only();
	#endif
#endif

		SyncSlave(m_SyncLock, corenum, offset);

		enable_cycle_counter();
		reset_cycle_counter();
		///////////////////
		// DO BENCHMARK! //
		///////////////////
		DO_BENCH_CORE1
		///////////////////
		disable_cycle_counter();
		cycles = read_cycle_counter();

		m_log->Write(FromCoRunners, LogNotice,
					 "CYCLECOUNT label: %s %s %s %s cores: %d core: %d cycle_count: %12u iteration: %u offset: %d",
					 EXP_LABEL, CONFIG_SERIES_STRING, CONFIG_BENCH_STRING,
					 BENCH_STRING_CORE1, NR_OF_CORES, corenum,
					 cycles, iter, offset);

		if (++iter % ITERATIONS_PER_STEP == 0)
			offset += OFFSET_STEP_SIZE;
	}
#else
	while (1) {
		// Basically, do nothing, or nothing much..
		countdown(100 * DELAY_STEP_COUNTDOWN);
	}
#endif
}

void CoRunners::RunCore2()
{
#if NR_OF_CORES >= 3
	RandomWrapper rand;
    u64 cycles;
	unsigned corenum = 2;
    unsigned int offset=0;

	/////////////////////
	// Benchmark INIT1 //
	/////////////////////
	BENCH_INIT1_CORE2
	/////////////////////

	/* Globally enable PMU */
	enable_pmu();

	unsigned iter=1;
	while (1) {
		/////////////////////
		// Benchmark INIT2 //
		/////////////////////
		BENCH_INIT2_CORE2
		/////////////////////

#ifndef DISABLE_CACHE
	#ifndef NO_CACHE_MGMT
		/* Slave only invalidates its own L1 cache */
		InvalidateDataCacheL1Only();
	#endif
#endif

		SyncSlave(m_SyncLock, corenum, offset);

		enable_cycle_counter();
		reset_cycle_counter();
		///////////////////
		// DO BENCHMARK! //
		///////////////////
		DO_BENCH_CORE2
		///////////////////
		disable_cycle_counter();
		cycles = read_cycle_counter();

		m_log->Write(FromCoRunners, LogNotice,
					 "CYCLECOUNT label: %s %s %s %s cores: %d core: %d cycle_count: %12u iteration: %u offset: %d",
					 EXP_LABEL, CONFIG_SERIES_STRING, CONFIG_BENCH_STRING,
					 BENCH_STRING_CORE2, NR_OF_CORES, corenum,
					 cycles, iter, offset);

		if (++iter % ITERATIONS_PER_STEP == 0)
			offset += OFFSET_STEP_SIZE;
	}
#else
	while (1) {
		// Basically, do nothing, or nothing much..
		countdown(100 * DELAY_STEP_COUNTDOWN);
	}
#endif
}

void CoRunners::RunCore3()
{
#if NR_OF_CORES >= 4
	RandomWrapper rand;
    u64 cycles;
	unsigned corenum = 3;
    unsigned int offset=0;

	/////////////////////
	// Benchmark INIT1 //
	/////////////////////
	BENCH_INIT1_CORE3
	/////////////////////

	/* Globally enable PMU */
	enable_pmu();

	unsigned iter=1;
	while (1) {
		/////////////////////
		// Benchmark INIT2 //
		/////////////////////
		BENCH_INIT2_CORE3
		/////////////////////

#ifndef DISABLE_CACHE
	#ifndef NO_CACHE_MGMT
		/* Slave only invalidates its own L1 cache */
		InvalidateDataCacheL1Only();
	#endif
#endif

		SyncSlave(m_SyncLock, corenum, offset);

		enable_cycle_counter();
		reset_cycle_counter();
		///////////////////
		// DO BENCHMARK! //
		///////////////////
		DO_BENCH_CORE3
		///////////////////
		disable_cycle_counter();
		cycles = read_cycle_counter();

		m_log->Write(FromCoRunners, LogNotice,
					 "CYCLECOUNT label: %s %s %s %s cores: %d core: %d cycle_count: %12u iteration: %u offset: %d",
					 EXP_LABEL, CONFIG_SERIES_STRING, CONFIG_BENCH_STRING,
					 BENCH_STRING_CORE3, NR_OF_CORES, corenum,
					 cycles, iter, offset);

		if (++iter % ITERATIONS_PER_STEP == 0)
			offset += OFFSET_STEP_SIZE;
	}
#else
	while (1) {
		// Basically, do nothing, or nothing much..
		countdown(100 * DELAY_STEP_COUNTDOWN);
	}
#endif
}
