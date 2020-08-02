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
#include <circle/logger.h>
#include <circle/alloc.h>
#include <circle/new.h>
#include <circle/spinlock.h>
#include <circle/types.h>
#include "randomwrapper.h"

extern "C" int write_atomic(unsigned, unsigned*);

static inline unsigned read_atomic(unsigned* ptr);

static const char FromCoRunners[] = "CoRunners";

static inline unsigned read_atomic(unsigned* ptr)
{
	unsigned val;
	asm volatile(
		"ldxr %x[val], [%[addr]]"
		: [val]"=r" (val)
		: [addr]"r" (ptr));
	asm volatile("dmb ish");
	return val;
}

CoRunners::CoRunners (CScreenDevice *pScreen, CMemorySystem *pMemorySystem)
:
	CMultiCoreSupport (pMemorySystem),
	m_pScreen (pScreen)
{
}

CoRunners::~CoRunners (void)
{
	m_pScreen = 0;
	free((void*) Array1);
	free((void*) m_data2);
	free((void*) m_data3);
	free((void*) m_data4);
}

void CoRunners::SyncMaster(CSpinLock& lock, unsigned nr)
{
	// CLogger::Get()->Write(FromCoRunners, LogNotice,
	// 					  "Master aquiring lock");

	lock.Acquire();

	// Set the flag for master, to indicate to the slaves that
	// we have set acquired the lock and will wait for the slaves
	// to start waiting
	if (!write_atomic(1, &m_CoreWaiting[nr][0]))
		CLogger::Get()->Write(FromCoRunners, LogNotice,
							  "Core0: failure to write to memory");

	boolean exit = false;
	while (!exit) {
		exit = true;
		exit = exit && read_atomic(&m_CoreWaiting[nr][1]);
		exit = exit && read_atomic(&m_CoreWaiting[nr][2]);
		exit = exit && read_atomic(&m_CoreWaiting[nr][3]);
	}

	for (int i=0; i<4; i++)
		if (!write_atomic(0, &m_CoreWaiting[nr][i]))
			CLogger::Get()->Write(FromCoRunners, LogNotice,
								  "Core0: failure to write to memory");

	lock.Release();
}

void CoRunners::SyncSlave(CSpinLock& lock, unsigned nr, unsigned corenum)
{
	if (!write_atomic(1, &m_CoreWaiting[nr][corenum]))
		CLogger::Get()->Write(FromCoRunners, LogNotice,
							  "Core%d: failure to write to memory", corenum);

	// First make sure that master has acquired the lock and is
	// already waiting
	while (!read_atomic(&m_CoreWaiting[nr][0]))
		;

	lock.Acquire();
	lock.Release();
}

void CoRunners::Run (unsigned corenum)
{
	if (corenum == 0) {
		for (int i=0; i<4; i++) {
			if (!write_atomic(0, &m_CoreWaiting[0][i]))
				CLogger::Get()->Write(FromCoRunners, LogNotice,
									  "Failure to write to memory!");
			if (!write_atomic(0, &m_CoreWaiting[1][i]))
				CLogger::Get()->Write(FromCoRunners, LogNotice,
									  "Failure to write to memory!");
		}
	}

	// // Test the read exclusive code
	// int local = 0x000000010000002A + corenum;
	// int res = write_atomic(local, &test_member);
	// if (res == 0) {
	// 	CLogger::Get()->Write(FromCoRunners, LogNotice,
	// 						  "Core %d wrote %d to memory", corenum, local);
	// } else {
	// 	CLogger::Get()->Write(FromCoRunners, LogNotice,
	// 						  "Core %d store exclusive was unsuccessful(%d)",
	// 						  corenum, res);
	// }

	// test_member = read_atomic(&test_member);

	// CLogger::Get()->Write(FromCoRunners, LogNotice,
	// 					  "Core %d read back %d from memory", corenum, test_member);

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
		CLogger::Get ()->Write(FromCoRunners, LogError,
							   "Illegal core number %d!", corenum);
	}
}

void CoRunners::RunCore0()
{
	RandomWrapper rand;
    u64 cycles;
	unsigned corenum = 0;

	CLogger::Get ()->Write(FromCoRunners, LogNotice,
						   "Core %d is requesting memory.", corenum);
	m_SpinLock.Acquire ();
	Array1 = (volatile int*) malloc(NUMELEMS * sizeof(int));
	m_SpinLock.Release ();
	if (Array1 == NULL)
		CLogger::Get()->Write(FromCoRunners, LogWarning,
							  "Core %d Array1 pointer is NULL!",
							  corenum);
	else
		CLogger::Get()->Write(FromCoRunners, LogNotice,
							  "Core %d Array1 pointer is not NULL.",
							  corenum);

	/* Globally enable PMU */
	enable_pmu();

	while (1) {
		SyncMaster(m_SyncLock[0], 0);

		bsort100_Initialize(Array1, &rand);
		enable_cycle_counter();
		reset_cycle_counter();
		bsort100_BubbleSort(Array1);
		disable_cycle_counter();
		cycles = read_cycle_counter();

		CLogger::Get ()->Write(FromCoRunners, LogNotice,
							   "Core%d: %lu cycles spent",
							   corenum, cycles);

		// let the temperature task run (only for core 0)
		CScheduler::Get ()->Yield ();

		SyncMaster(m_SyncLock[1], 1);
	}
}

void CoRunners::RunCore1()
{
    u64 cycles;
	unsigned corenum = 1;
	CLogger::Get ()->Write(FromCoRunners, LogNotice,
						   "Core %d is requesting memory.", corenum);
	m_SpinLock.Acquire ();
	m_data2 = new bigstruct_t[SYNBENCH_DATASIZE];
	m_SpinLock.Release ();
	if (m_data2 == NULL)
		CLogger::Get()->Write(FromCoRunners, LogWarning,
							  "Core %d m_data2 pointer is NULL!",
							  corenum);
	else
		CLogger::Get()->Write(FromCoRunners, LogNotice,
							  "Core %d m_data2 pointer is not NULL.",
							  corenum);

	/* Globally enable PMU */
	enable_pmu();

	while (1) {
		SyncSlave(m_SyncLock[0], 0, corenum);

		enable_cycle_counter();
		reset_cycle_counter();
		array_write_linear(m_data2);
		disable_cycle_counter();
		cycles = read_cycle_counter();

		CLogger::Get ()->Write(FromCoRunners, LogNotice,
							   "Core%d: %lu cycles spent",
							   corenum, cycles);

		SyncSlave(m_SyncLock[1], 1, corenum);
	}
}

void CoRunners::RunCore2()
{
    u64 cycles;
	unsigned corenum = 2;
	CLogger::Get ()->Write(FromCoRunners, LogNotice,
						   "Core %d is requesting memory.", corenum);
	m_SpinLock.Acquire ();
	m_data3 = new bigstruct_t[SYNBENCH_DATASIZE];
	m_SpinLock.Release ();
	if (m_data3 == NULL)
		CLogger::Get()->Write(FromCoRunners, LogWarning,
							  "Core %d m_data3 pointer is NULL!",
							  corenum);
	else
		CLogger::Get()->Write(FromCoRunners, LogNotice,
							  "Core %d m_data3 pointer is not NULL.",
							  corenum);

	/* Globally enable PMU */
	enable_pmu();

	while (1) {
		SyncSlave(m_SyncLock[0], 0, corenum);

		enable_cycle_counter();
		reset_cycle_counter();
		array_write_linear(m_data3);
		disable_cycle_counter();
		cycles = read_cycle_counter();

		CLogger::Get ()->Write(FromCoRunners, LogNotice,
							   "Core%d: %lu cycles spent",
							   corenum, cycles);

		SyncSlave(m_SyncLock[1], 1, corenum);
	}
}

void CoRunners::RunCore3()
{
    u64 cycles;
	unsigned corenum = 3;
	CLogger::Get ()->Write(FromCoRunners, LogNotice,
						   "Core %d is requesting memory.", corenum);
	m_SpinLock.Acquire ();
	m_data4 = new bigstruct_t[SYNBENCH_DATASIZE];
	m_SpinLock.Release ();
	if (m_data4 == NULL)
		CLogger::Get()->Write(FromCoRunners, LogWarning,
							  "Core %d m_data4 pointer is NULL!",
							  corenum);
	else
		CLogger::Get()->Write(FromCoRunners, LogNotice,
							  "Core %d m_data4 pointer is not NULL.",
							  corenum);

	/* Globally enable PMU */
	enable_pmu();

	while (1) {
		SyncSlave(m_SyncLock[0], 0, corenum);

		enable_cycle_counter();
		reset_cycle_counter();
		array_write_linear(m_data4);
		disable_cycle_counter();
		cycles = read_cycle_counter();

		CLogger::Get ()->Write(FromCoRunners, LogNotice,
							   "Core%d: %lu cycles spent",
							   corenum, cycles);

		SyncSlave(m_SyncLock[1], 1, corenum);
	}
}
