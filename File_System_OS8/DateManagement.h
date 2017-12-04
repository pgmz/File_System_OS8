
#include <stdio.h>
#include <time.h>

struct DATE {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
};

unsigned int datetoint(struct DATE date);
int inttodate(struct DATE *date,unsigned int val);
// Obtiene la fecha y hora actual del sistema y la 
// empaqueta en un entero de 32 bits
unsigned int currdatetimetoint();