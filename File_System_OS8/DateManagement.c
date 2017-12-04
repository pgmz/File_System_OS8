
#include "DateManagement.h"

unsigned int datetoint(struct DATE date)
{
	unsigned int val=0;

	val=date.year-1970;
	val<<=4;
	val|=date.month;
	val<<=5;
	val|=date.day;
	val<<=5;
	val|=date.hour;
	val<<=6;
	val|=date.min;
	val<<=6;
	val|=date.sec;
	
	return(val);
}

int inttodate(struct DATE *date,unsigned int val)
{
	date->sec=val&0x3F;
	val>>=6;
	date->min=val&0x3F;
	val>>=6;
	date->hour=val&0x1F;
	val>>=5;
	date->day=val&0x1F;
	val>>=5;
	date->month=val&0x0F;
	val>>=4;
	date->year=(val&0x3F) + 1970;
	return(1);
}


// Obtiene la fecha y hora actual del sistema y la 
// empaqueta en un entero de 32 bits
unsigned int currdatetimetoint()
{
	struct tm *tm_ptr;
	time_t the_time;
	
	struct DATE now;

	// Llamada al sistema para obtener la fecha/hora actual
	// y guardar el resultado en the_time
	(void) time(&the_time);
	tm_ptr=gmtime(&the_time);

	// Poner la fecha y hora obtenida en la estructura TIME
	now.year=tm_ptr->tm_year-70;
	now.month=tm_ptr->tm_mon+1;
	now.day=tm_ptr->tm_mday;
	now.hour=tm_ptr->tm_hour;
	now.min=tm_ptr->tm_min;
	now.sec=tm_ptr->tm_sec;
	// Convertirlo a un entero de 32 bits y regresar el 
// resultado
	return(datetoint(now));
}