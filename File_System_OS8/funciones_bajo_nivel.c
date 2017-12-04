#include "funciones_bajo_nivel.h"

#define SFIP			2
#define HIP				0
#define CIP				0

#define SEC_X_TRACK		27

static int secboot_en_memoria = 0;
struct SECBOOTPART secboot;

int vdwriteseclog(int seclog,char *buffer)
{
	// Calcular a partir de seclog
	//	- cilindro
	int cilindro = ((seclog + SFIP - 1) + HIP*SEC_X_TRACK)/(SEC_X_TRACK*HEADS) + CIP;
	//	- superficie
	int superficie = ((seclog + SFIP - 1)/(SEC_X_TRACK + HIP))%HEADS;
	// 	- sector_fisico
	int sector_fisico = ((seclog + SFIP - 1)%SEC_X_TRACK) + 1;
	
	vdwritesector(0,superficie,cilindro,sector_fisico,1,buffer);
}

int vdreadseclog(int seclog,char *buffer)
{
	// Calcular a partir de seclog
	//	- cilindro
	int cilindro = ((seclog + SFIP - 1) + HIP*SEC_X_TRACK)/(SEC_X_TRACK*HEADS) + CIP;
	//	- superficie
	int superficie = ((seclog + SFIP - 1)/(SEC_X_TRACK + HIP))%HEADS;
	// 	- sector_fisico
	int sector_fisico = (seclog + SFIP - 1)%SEC_X_TRACK + 1;
	
	vdreadsector(0,superficie,cilindro,sector_fisico,1,buffer);
}

// Funciones para la lectura y escritura de bloques, a partir 
// de un número de bloques

int writeblock(int nblock,char *buffer)
{
	int slad; // Sector lógico del área de datos
	int secs_x_bloque;
	int i;

	int result;
	
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}
	
	if (result == -1){
		return -1;
	}

	// Obtener los datos del sector de boot de la partición
	// para:
	// 	1.- Determinar en que sector lógico inicia el área
	// 	de bloques de datos, y guardarlo en la variable slad
	//
	slad = secboot.sec_mapa_bits_area_nodos_i + secboot.sec_mapa_bits_bloques + secboot.sec_tabla_nodos_i + 1;
	//	2.- Obtener el número de sectores por bloque y guardarlo
	// 	en la variable secs_x_bloque
	secs_x_bloque = secboot.sec_x_bloque;
	
	// Calcular en que sector lógico inicia el bloque
	int seclog=slad+(nblock-1)*secs_x_bloque;

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

	int result;
	
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}
	
	if (result == -1){
		return -1;
	}

	// Obtener los datos del sector de boot de la partición
	// para:
	// 	1.- Determinar en que sector lógico inicia el área
	// 	de bloques de datos, y guardarlo en la variable slad
	//
	slad = secboot.sec_mapa_bits_area_nodos_i + secboot.sec_mapa_bits_bloques + secboot.sec_tabla_nodos_i + 1;
	//	2.- Obtener el número de sectores por bloque y guardarlo
	// 	en la variable secs_x_bloque
	secs_x_bloque = secboot.sec_x_bloque;
	
	// Calcular en que sector lógico inicia el bloque
	int seclog=slad+(nblock-1)*secs_x_bloque;

	// Escribir en todos los sectores lógicos del bloque
	for(i=0;i<secs_x_bloque;i++)
	{	
		vdreadseclog(seclog,buffer);
		seclog++;
		buffer+=512;
	}
}

int getStartOfDataMap(){

	int result;
	
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}
	
	if (result == -1){
		return -1;
	}
	
	return secboot.sec_mapa_bits_area_nodos_i + 1;

}

int getStartOfDataArea(){
	
	int result;
	
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}
	
	if (result == -1){
		return -1;
	}
	
	return secboot.sec_mapa_bits_area_nodos_i + secboot.sec_mapa_bits_bloques + secboot.sec_tabla_nodos_i + 1;
}
