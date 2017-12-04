
#include "DateManagement.h"

struct DATE date;

int currdatetimetoint(){

	int dateInInt;
	struct tm *loctime;
	time_t sec;
    sec = time (NULL);
    
   	/* Convert it to local time representation. */
  	loctime = localtime (&sec);
  	
  	date.year = loctime->tm_year;
	date.month = loctime->tm_mon;
	date.day = loctime->tm_wday;
	date.hour = loctime->tm_hour;
	date.min = loctime->tm_min;
	date.sec = loctime->tm_sec;
	 
	dateInInt = (date.sec & 0x3F)
	| ((date.min & 0x3f) << 6) | ((date.hour & 0x1F) << 12);
	 
	dateInInt = dateInInt
	| ((date.day & 0x1F) << 17) | ((date.month & 0xF) << 22)
	| (((date.year - 1970) & 0x3F) << 26);
	
	return dateInInt;
}