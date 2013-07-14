#include "GP_Time.h"

#ifndef _WIN32_WCE
#   if defined(_WIN32)
#       include <sys/timeb.h>
#       include <time.h>
#   elif defined(_VXWORKS)
#		include <time.h>
#	else
#		include <sys/time.h>
#   endif
#endif

GP_Time::GP_Time() :
    _usec(0)
{
}

GP_Time
GP_Time::now()
{
#if defined(_WIN32_WCE)
    //
    // Note that GetTickCount returns the number of ms since the
    // device was started. Time cannot be used to represent an
    // absolute time on CE since GetLocalTime doesn't have millisecond
    // resolution.
    //
    return GP_Time(static_cast<Int64>(GetTickCount()) * 1000);
#elif defined(_WIN32)
    struct _timeb tb;
    _ftime(&tb);
    return GP_Time(tb.time * static_cast<Int64>(1000000) + tb.millitm * static_cast<Int64>(1000));
#else
#	if defined(_VXWORKS)
	//VxWorks defined.
//	time_t t;
//	time(&t);
//	return Time( static_cast<Int64>(t%86400)* 1000000 );
	timespec tp;
	clock_gettime(CLOCK_REALTIME,&tp);
	return GP_Time(tp.tv_sec * static_cast<Int64>(1000000) + tp.tv_nsec/1000);
#	elif defined(_ACOREOS)
	T_UDWORD ticks;
	T_UDWORD msecpertick;
	ACoreOs_clock_get(ACOREOS_CLOCK_GET_TICKS_SINCE_BOOT,&ticks);
	ACoreOs_clock_get(ACOREOS_CLOCK_GET_MICROSEC_PER_TICK,&msecpertick);
	return GP_Time(ticks*static_cast<Int64>(1000000)*msecpertick);
#	else
    struct timeval tv;
    gettimeofday(&tv, 0);
    return GP_Time(tv.tv_sec * static_cast<Int64>(1000000) + tv.tv_usec);

#	endif
#endif
}

GP_Time
GP_Time::seconds(Int64 t)
{
    return GP_Time(t * static_cast<Int64>(1000000));
}

GP_Time
GP_Time::milliSeconds(Int64 t)
{
    return GP_Time(t * static_cast<Int64>(1000));
}

GP_Time
GP_Time::microSeconds(Int64 t)
{
    return GP_Time(t);
}

#ifndef _WIN32
GP_Time::operator timeval() const
{
    timeval tv;
    tv.tv_sec = static_cast<long>(_usec / 1000000);
    tv.tv_usec = static_cast<long>(_usec % 1000000);
    return tv;
}
#endif

Int64
GP_Time::toSeconds() const
{
    return _usec / 1000000;
}

Int64
GP_Time::toMilliSeconds() const
{
    return _usec / 1000;
}

Int64
GP_Time::toMicroSeconds() const
{
    return _usec;
}

double
GP_Time::toSecondsDouble() const
{
    return _usec / 1000000.0;
}

double
GP_Time::toMilliSecondsDouble() const
{
    return _usec / 1000.0;
}

double
GP_Time::toMicroSecondsDouble() const
{
    return static_cast<double>(_usec);
}

GP_Time::GP_Time(Int64 usec) :
    _usec(usec)
{
}
