#pragma once
//
// This file is part of smart_table, a library and tools implementing
// data structures which manage data (add, remove, query) in tabular form.
//
// Copyright (C) 2009 Filip Fodemski
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// More information at http://www.gnu.org/licenses/lgpl.html
//
// $Id: hr_timer.h 52 2009-11-18 10:27:28Z filipek $
//

#include "targetver.h"
#include "stlib_selectlib.h"

#include "windows.h"

#include <iostream>

namespace smart_table
{

namespace tools
{

///////////////////////////////////////////////////////////////////////////////
struct hr_timer
{
	typedef __int64 time_type;

    LARGE_INTEGER start_time;
    LARGE_INTEGER stop_time;

	hr_timer()
		: start_time()
		, stop_time() {}

    inline void start(void)
	{
        QueryPerformanceCounter(&start_time);
    }
    inline void stop(void)
	{
        QueryPerformanceCounter(&stop_time);
    }
    inline double duration(void) const
    {
        const time_type freq = frequency();
		const double duration =
			static_cast<double>(stop_time.QuadPart - start_time.QuadPart)
				/ static_cast<double>(freq);

		// in seconds
        return duration;
    }
	inline static double now(void)
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
		return static_cast<double>(now.QuadPart);
    }
	inline static time_type frequency(void)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
		return freq.QuadPart;
    }
};

inline std::ostream& operator<<(
	std::ostream& os
	, const hr_timer& tmr)
{
	return os << tmr.duration() << " seconds";
}

} // namespace tools

} // namespace smart_table
