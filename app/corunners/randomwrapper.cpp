//
// randomwrapper.cpp
//
// Subclass derived from CBcmRandomNumberGenerator, with the
// exact same functionality but also provides a wrapper for
// usage in C.
// Additionally, two extra methods are defined for getting a
// random float:
//  - float GetFloat(void) => will interpret a u32 random number
//                            as a float
//
//  - float GetSmall Float(void) => will interpret a u32 random number
//                                  as a float, but will also do some
//                                  bit manipulation on the u32 number
//                                  to make the float fall within the
//                                  [-1.0, 1.0] interval.
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2016  R. Stange <rsta2@o2online.de>
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
#include "randomwrapper.h"


float RandomWrapper::GetFloat(void)
{
	return ConvertNumber(GetNumber(), false);
}

float RandomWrapper::GetSmallFloat(void)
{
	return ConvertNumber(GetNumber(), true);
}

float RandomWrapper::ConvertNumber(u32 number, bool shrink)
{
	float* f;

	if (shrink) {
		// Do some bit manipulation to make sure interpreted float
		// falls between [-1.0, 1.0]

		// The biased exponent, bits 30-23, must be smaller than 127.
		// That means bit 30 must be 0, and if all bits 29-24 are 1, then
		// bit 23 must also be zero.

		// Turn off bit 30 (biased exponent MSB):
		number = number & 0xbfffffff;

		// maybe turn off bit 23:
		u32 significand = number & 0x007fffff;
		if (significand != 0) {
			// significand is greater than 0, maybe turn off bit 23
			u32 exponent = (number & 0x7f800000) >> 23;
			if (exponent >= 127) {
				// Turn off bit 23 (biased exponent LSB)
				number = number & 0xff7fffff;
			}
		}
	}

	f = (float*) &number;

	return(*f);
}

// These are the wrapper functions to make the API available from C
u32 get_number(RandomWrapper* rand)
{
	return rand->GetNumber();
}

u32 get_float(RandomWrapper* rand)
{
	return rand->GetFloat();
}

u32 get_small_float(RandomWrapper* rand)
{
	return rand->GetSmallFloat();
}
