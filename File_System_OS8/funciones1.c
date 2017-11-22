#include "vdisk.h"

int vdwriteseclog(int seclog,char *buffer)
{
	// Calcular a partir de seclog
	//	- cilindro
	//	- superficie
	// 	- sector_fisico
	vdwritesector(0,obtenerSuperficie(seclog),obtenerCilindro(seclog),obtenerSectorFisico(seclog),1,(char *) &buffer);
}

int vdreadseclog(int seclog,char *buffer)
{
	// Calcular a partir de seclog
	//	- cilindro
	//	- superficie
	// 	- sector_fisico
	vdreadsector(0,obtenerSuperficie(seclog),obtenerCilindro(seclog),obtenerSectorFisico(seclog),1,(char *) &buffer);
}

// Funciones para la lectura y escritura de bloques, a partir 
// de un número de bloques

int writeblock(int nblock,char *buffer)
{
	int slad; // Sector lógico del área de datos
	int secs_x_bloque;
	int i;
	int seclog;

	// Obtener los datos del sector de boot de la partición
	// para:
	// 	1.- Determinar en que sector lógico inicia el área
	// 	de bloques de datos, y guardarlo en la variable slad
	//
	//	2.- Obtener el número de sectores por bloque y guardarlo
	// 	en la variable secs_x_bloque
	
	// Calcular en que sector lógico inicia el bloque
	seclog=slad+(nblock-1)*secs_x_bloque;

	// Escribir en todos los sectores lógicos del bloque
	for(i=0;i<secs_x_bloque;i++)
	{	
		vdwriteseclog(seclog,buffer);
		seclog++;
		buffer+=512;
	}
}

int readblock(int nblock,char *buffer)
{
	int slad; // Sector lógico del área de datos
	int secs_x_bloque;
	int i;
	int seclog;
	// Obtener los datos del sector de boot de la partición
	// para:
	// 	1.- Determinar en que sector lógico inicia el área
	// 	de bloques de datos, y guardarlo en la variable slad
	//
	//	2.- Obtener el número de sectores por bloque y guardarlo
	// 	en la variable secs_x_bloque
	
	// Calcular en que sector lógico inicia el bloque
	seclog=slad+(nblock-1)*secs_x_bloque;

	// Escribir en todos los sectores lógicos del bloque
	for(i=0;i<secs_x_bloque;i++)
	{	
		vdreadseclog(seclog,buffer);
		seclog++;
		buffer+=512;
	}
}

int obtenerSectorFisico(int seclog)
{
	return (((seclog+SFIP-1)%SEC_X_TRACK)+1);
}

int obtenerCilindro(int seclog)
{
	return (((seclog+SFIP-1)+(HIP*SEC_X_TRACK))/(SEC_X_TRACK*HEADS));
}

int obtenerSuperficie(int seclog)
{
	return (((seclog+SFIP-1)/(SEC_X_TRACK+HIP))%HEADS);
}