#include "DateTime.h"

using namespace ROG;


DateTime::DateTime()	{
	TimeInit();
}

int DateTime::TimeInit()	{
	currentDate = time(NULL);
	if (currentDate < 1)	{
		currentDate = 0;
		cout	<< "AbstractDateTime: Cant get current date from time(NULL)\n";
		return -1;
	}

#ifdef _WIN32
	QueryPerformanceFrequency(&tick_accuracy);
	QueryPerformanceCounter(&tick_count);

	system_time = (unsigned long)(tick_count.QuadPart / (tick_accuracy.QuadPart / CLOCK_ACCURACY));
#else
	system_time = time(NULL);
	
#endif
	return system_time;
}




/*
int DateTime::DateStr(char* buf, int maxlen)	{
	int size = 0;
	if (currentDate)	{
		size = (int)strftime(buf, maxlen, "%c", daterep);
	}
	return size == 0;
}
*/

/*

A
	 name of weekday
a
	 abbreviated name of weekday
B
	 name of month
b
	 abbreviated name of month
c
	 local date and time representation
d
	 day of month [01-31]
H
	 hour (24-hour clock) [00-23]
I
	 hour (12-hour clock) [01-12]
j
	 day of year [001-366]
M
	 minute [00-59]
m
	 month [01-12]
p
	 local equivalent of "AM" or "PM"
S
	 second [00-61]
U
	 week number of year (Sunday as 1st day of week) [00-53]
W
	 week number of year (Monday as 1st day of week) [00-53]
w
	 weekday (Sunday as 0) [0-6]
X
	 local time representation
x
	 local date representation
Y
	 year with century
y
	 year without century [00-99]
Z
	 name (if any) of time zone
%
	 % */
	 
	 
	 


