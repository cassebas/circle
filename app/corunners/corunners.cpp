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
	free((void*) Array1);
	free((void*) m_data2);
	free((void*) m_data3);
	free((void*) m_data4);
	free((void*) m_randidx2);
	free((void*) m_randidx3);
	free((void*) m_randidx4);
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
		exit = exit && read_bit_atomic(1, &m_CoreWaiting);
		exit = exit && read_bit_atomic(2, &m_CoreWaiting);
		exit = exit && read_bit_atomic(3, &m_CoreWaiting);
	}

	for (int i=1; i<4; i++)
		if (write_bit_atomic(i, false, &m_CoreWaiting))
			m_log->Write(FromCoRunners, LogWarning,
						 "Core0: failure to write to memory");

	lock.Release();

	// Reset waiting status for master
	if (write_bit_atomic(4, false, &m_CoreWaiting))
		m_log->Write(FromCoRunners, LogWarning,
					 "Core0: failure to write to memory");
}

void CoRunners::SyncSlave(CSpinLock& lock, unsigned corenum)
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

	// Maybe wait a while (create time offset in starting time co-runner)
	// countdown(1000000000);
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

#ifdef BENCH_CONFIG_CORE0_3_1
	// Disparity: initialization part 1, allocate memory for data
	// and make seed
	// int seed = corenum + 1;
	// srand(seed);
	int width=DISPARITY_INPUTSIZE, height=DISPARITY_INPUTSIZE;
    int WIN_SZ=4, SHIFT=8;
    I2D* srcImage1 = iMallocHandle(width, height);
    I2D* srcImage2 = iMallocHandle(width, height);
#endif
#ifdef BENCH_CONFIG_CORE0_2_1
	Array1 = (volatile int*) new (HEAP_HIGH) int[NUMELEMS];
#endif

	m_log->Write(FromCoRunners, LogDebug,
				 "Core %d is requesting memory.", corenum);
	m_SpinLock.Acquire ();
	m_SpinLock.Release ();
	if (Array1 == NULL)
		m_log->Write(FromCoRunners, LogWarning,
					 "Core %d Array1 pointer is NULL!",
					 corenum);
	else
		m_log->Write(FromCoRunners, LogDebug,
					 "Core %d Array1 pointer is not NULL.",
					 corenum);

	/* Globally enable PMU */
	enable_pmu();

	unsigned iter=0;
	while (1) {
#ifdef BENCH_CONFIG_CORE0_2_1
		/* Maybe initialize the bsort100 array with random nrs (each iteration) */
		bsort100_Initialize(Array1, &rand);
#endif
#ifdef BENCH_CONFIG_CORE3_3_1
	// Disparity initialization part 2: fill the data with random numbers
	for (int i=0; i<(width*height); i++) {
		srcImage1->data[i] = rand.get_number() % 256;
		srcImage2->data[i] = rand.get_number() % 256;
	}
#endif
#if defined BENCH_CONFIG_CORE0_1_3 || defined BENCH_CONFIG_CORE0_1_4
		array_access_randomize(m_randidx1, &rand);
#endif
		SyncMaster(m_SyncLock);

		enable_cycle_counter();
		reset_cycle_counter();
		DO_BENCH_CORE0
		disable_cycle_counter();
		cycles = read_cycle_counter();

		m_log->Write(FromCoRunners, LogNotice,
					 "CYCLECOUNT label: %s %s %s %s cores: %d core: %d cycle_count: %12u iteration: %u offset: %d",
					 EXP_LABEL, CONFIG_SERIES_STRING, CONFIG_BENCH_STRING,
					 BENCH_STRING_CORE0, NR_OF_CORES, corenum,
					 cycles, ++iter, 0);

		// let the temperature task run (only for core 0)
		CScheduler::Get ()->Yield ();
	}
}

void CoRunners::RunCore1()
{
	RandomWrapper rand;
    u64 cycles;
	unsigned corenum = 1;

	m_log->Write(FromCoRunners, LogDebug,
				 "Core %d is requesting memory.", corenum);

	m_SpinLock.Acquire ();
	m_data2 = new bigstruct_t[SYNBENCH_DATASIZE];
	m_randidx2 = new int[SYNBENCH_DATASIZE];
	m_SpinLock.Release ();
	if (m_data2 == NULL)
		m_log->Write(FromCoRunners, LogWarning,
					 "Core %d m_data2 pointer is NULL!",
					 corenum);
	else
		m_log->Write(FromCoRunners, LogDebug,
					 "Core %d m_data2 pointer is not NULL.",
					 corenum);

	/* Globally enable PMU */
	enable_pmu();

	unsigned iter=0;
	while (1) {
#if defined BENCH_CONFIG_CORE1_1_3 || defined BENCH_CONFIG_CORE1_1_4
		array_access_randomize(m_randidx2, &rand);
#endif
		SyncSlave(m_SyncLock, corenum);

		enable_cycle_counter();
		reset_cycle_counter();
		array_write_random(m_data2, m_randidx2);
		disable_cycle_counter();
		cycles = read_cycle_counter();

		m_log->Write(FromCoRunners, LogNotice,
					 "CYCLECOUNT label: %s %s %s %s cores: %d core: %d cycle_count: %12u iteration: %u offset: %d",
					 EXP_LABEL, CONFIG_SERIES_STRING, CONFIG_BENCH_STRING,
					 BENCH_STRING_CORE1, NR_OF_CORES, corenum,
					 cycles, ++iter, 0);
	}
}

void CoRunners::RunCore2()
{
	RandomWrapper rand;
    u64 cycles;
	unsigned corenum = 2;

	m_log->Write(FromCoRunners, LogDebug,
				 "Core %d is requesting memory.", corenum);

	m_SpinLock.Acquire ();
	m_data3 = new bigstruct_t[SYNBENCH_DATASIZE];
	m_randidx3 = new int[SYNBENCH_DATASIZE];
	m_SpinLock.Release ();
	if (m_data3 == NULL)
		m_log->Write(FromCoRunners, LogWarning,
					 "Core %d m_data3 pointer is NULL!",
					 corenum);
	else
		m_log->Write(FromCoRunners, LogDebug,
					 "Core %d m_data3 pointer is not NULL.",
					 corenum);

	/* Globally enable PMU */
	enable_pmu();

	unsigned iter=0;
	while (1) {
#if defined BENCH_CONFIG_CORE2_1_3 || defined BENCH_CONFIG_CORE2_1_4
		array_access_randomize(m_randidx3, &rand);
#endif
		SyncSlave(m_SyncLock, corenum);

		enable_cycle_counter();
		reset_cycle_counter();
		array_write_random(m_data3, m_randidx3);
		disable_cycle_counter();
		cycles = read_cycle_counter();

		m_log->Write(FromCoRunners, LogNotice,
					 "CYCLECOUNT label: %s %s %s %s cores: %d core: %d cycle_count: %12u iteration: %u offset: %d",
					 EXP_LABEL, CONFIG_SERIES_STRING, CONFIG_BENCH_STRING,
					 BENCH_STRING_CORE2, NR_OF_CORES, corenum,
					 cycles, ++iter, 0);
	}
}

void CoRunners::RunCore3()
{
	RandomWrapper rand;
    u64 cycles;
	unsigned corenum = 3;

	m_log->Write(FromCoRunners, LogDebug,
				 "Core %d is requesting memory.", corenum);

	m_SpinLock.Acquire ();
	m_data4 = new bigstruct_t[SYNBENCH_DATASIZE];
	m_randidx4 = new int[SYNBENCH_DATASIZE];
	m_SpinLock.Release ();
	if (m_data4 == NULL)
		m_log->Write(FromCoRunners, LogWarning,
					 "Core %d m_data4 pointer is NULL!",
					 corenum);
	else
		m_log->Write(FromCoRunners, LogDebug,
					 "Core %d m_data4 pointer is not NULL.",
					 corenum);

	/* Globally enable PMU */
	enable_pmu();

	unsigned iter=0;
	while (1) {
#if defined BENCH_CONFIG_CORE3_1_3 || defined BENCH_CONFIG_CORE3_1_4
		array_access_randomize(m_randidx4, &rand);
#endif
		SyncSlave(m_SyncLock, corenum);

		enable_cycle_counter();
		reset_cycle_counter();
		array_write_random(m_data4, m_randidx4);
		disable_cycle_counter();
		cycles = read_cycle_counter();

		m_log->Write(FromCoRunners, LogNotice,
					 "CYCLECOUNT label: %s %s %s %s cores: %d core: %d cycle_count: %12u iteration: %u offset: %d",
					 EXP_LABEL, CONFIG_SERIES_STRING, CONFIG_BENCH_STRING,
					 BENCH_STRING_CORE3, NR_OF_CORES, corenum,
					 cycles, ++iter, 0);
	}
}
