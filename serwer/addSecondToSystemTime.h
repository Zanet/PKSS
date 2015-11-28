#include <time.h>
#include <cstdio>

void addSecondsToSystemTime(int seconds)
{
	time_t timer;
	time(&timer); //get the current system time
	
	printf("Current time is: %s\n", ctime(&timer));

	//timer+=600; // add 10 minutes to system time
	
    timer += seconds;
    stime(&timer);	
    time(&timer);
	printf("Current time after modification is: %s\n", ctime(&timer));

	return 0;
}	
