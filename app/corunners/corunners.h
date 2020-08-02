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

#include <circle/multicore.h>
#include <circle/screen.h>
#include <circle/memory.h>
#include <circle/types.h>
extern "C" {
	#include "armv8_pm.h"
	#include "synthetic_bench.h"
	#include "malardalen.h"
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
	void SyncMaster(CSpinLock&, unsigned);
	void SyncSlave(CSpinLock&, unsigned, unsigned);

private:
	CScreenDevice *m_pScreen;
	CSpinLock m_SpinLock;
	CSpinLock m_SyncLock[2];
	unsigned m_CoreWaiting[2][4];
	volatile bigstruct_t* m_data1;
	volatile bigstruct_t* m_data2;
	volatile bigstruct_t* m_data3;
	volatile bigstruct_t* m_data4;
	volatile int* Array1;
	volatile int* Array2;
	volatile int* Array3;
	volatile int* Array4;
};

#endif
