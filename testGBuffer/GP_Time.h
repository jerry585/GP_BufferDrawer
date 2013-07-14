#ifndef _GP_TIME_H_
#define _GP_TIME_H_

#ifdef _VXWORKS
	#include <sys/times.h>
#endif

//
// Int64 类型定义
//
#if defined(_MSC_VER)
typedef __int64 Int64;
#else
typedef long long Int64;
#endif

class GP_Time
{
public:

    GP_Time();

    // No copy constructor and assignment operator necessary. The
    // automatically generated copy constructor and assignment
    // operator do the right thing.
    
    static GP_Time now();
    static GP_Time seconds(Int64);
    static GP_Time milliSeconds(Int64);
    static GP_Time microSeconds(Int64);
    
#ifndef _WIN32
    operator timeval() const;
#endif

    Int64 toSeconds() const;
    Int64 toMilliSeconds() const;
    Int64 toMicroSeconds() const;

    double toSecondsDouble() const;
    double toMilliSecondsDouble() const;
    double toMicroSecondsDouble() const;

    GP_Time operator-() const
    {
	return GP_Time(-_usec);
    }

    GP_Time operator-(const GP_Time& rhs) const
    {
	return GP_Time(_usec - rhs._usec);
    }

    GP_Time operator+(const GP_Time& rhs) const
    {
	return GP_Time(_usec + rhs._usec);
    }

    GP_Time& operator+=(const GP_Time& rhs)
    {
	_usec += rhs._usec;
	return *this;
    }

    GP_Time& operator-=(const GP_Time& rhs)
    {
	_usec -= rhs._usec;
	return *this;
    }

    bool operator<(const GP_Time& rhs) const
    {
	return _usec < rhs._usec;
    }

    bool operator<=(const GP_Time& rhs) const
    {
	return _usec <= rhs._usec;
    }

    bool operator>(const GP_Time& rhs) const
    {
	return _usec > rhs._usec;
    }

    bool operator>=(const GP_Time& rhs) const
    {
	return _usec >= rhs._usec;
    }

    bool operator==(const GP_Time& rhs) const
    {
	return _usec == rhs._usec;
    }

    bool operator!=(const GP_Time& rhs) const
    {
	return _usec != rhs._usec;
    }

    GP_Time& operator*=(const GP_Time& rhs)
    {
	_usec *= rhs._usec;
	return *this;
    }

    GP_Time operator*(const GP_Time& rhs) const
    {
	GP_Time t;
	t._usec = _usec * rhs._usec;
	return t;
    }

    GP_Time& operator/=(const GP_Time& rhs)
    {
	_usec /= rhs._usec;
	return *this;
    }

    GP_Time operator/(const GP_Time& rhs) const
    {
	GP_Time t;
	t._usec = _usec / rhs._usec;
	return t;
    }

    GP_Time& operator*=(int rhs)
    {
	_usec *= rhs;
	return *this;
    }

    GP_Time operator*(int rhs) const
    {
	GP_Time t;
	t._usec = _usec * rhs;
	return t;
    }

    GP_Time& operator/=(int rhs)
    {
	_usec /= rhs;
	return *this;
    }

    GP_Time operator/(int rhs) const
    {
	GP_Time t;
	t._usec = _usec / rhs;
	return t;
    }

    GP_Time& operator*=(Int64 rhs)
    {
	_usec *= rhs;
	return *this;
    }

    GP_Time operator*(Int64 rhs) const
    {
	GP_Time t;
	t._usec = _usec * rhs;
	return t;
    }

    GP_Time& operator/=(Int64 rhs)
    {
	_usec /= rhs;
	return *this;
    }

    GP_Time operator/(Int64 rhs) const
    {
	GP_Time t;
	t._usec = _usec / rhs;
	return t;
    }

    GP_Time& operator*=(double rhs)
    {
	_usec = static_cast<Int64>(static_cast<double>(_usec) * rhs);
	return *this;
    }

    GP_Time operator*(double rhs) const
    {
	GP_Time t;
	t._usec = static_cast<Int64>(static_cast<double>(_usec) * rhs);
	return t;
    }

    GP_Time& operator/=(double rhs)
    {
	_usec = static_cast<Int64>(static_cast<double>(_usec) / rhs);
	return *this;
    }

    GP_Time operator/(double rhs) const
    {
	GP_Time t;
	t._usec = static_cast<Int64>(static_cast<double>(_usec) / rhs);
	return t;
    }

private:

    GP_Time(Int64);

    Int64 _usec;
};

#endif