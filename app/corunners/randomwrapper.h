//
// randomwrapper.h
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
#ifndef _randomwrapper_h
#define _randomwrapper_h

#include <circle/types.h>

#ifdef __cplusplus
	#include <circle/bcmrandom.h>

	/// CBcmRandomNumberGenerator is the driver for the built-in
	// hardware random number generator
	class RandomWrapper : public CBcmRandomNumberGenerator
	{
	public:
		/// \return Random float (32-bit)
		float GetFloat(void);
		/// \return Random float ([-1.0, 1.0])
		float GetSmallFloat(void);

	private:
		float ConvertNumber(u32, bool);
	};
#else
	typedef struct RandomWrapper RandomWrapper;
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// This is the wrapper function to make the API available from C
	extern u32 get_number(RandomWrapper*);
	extern u32 get_float(RandomWrapper*);
	extern u32 get_small_float(RandomWrapper*);

#ifdef __cplusplus
}
#endif

#endif /* _randomwrapper_h */
