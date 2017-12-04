#include <stdio.h>
#include <string.h>
#include "vdisk.h"


#define CYLINDERS 200
#define HEADS 8
#define SECTORS 27

int main()
{
	struct SECBOOTPART sbp;
	struct MBR mbr;
	
	memset(&mbr,0,512);
	memset(&sbp,0,512);
	
	strcpy(sbp.nombre_particion,"PRUEBA12");
	sbp.sec_inicpart=0;
	sbp.sec_res=1;
	sbp.sec_mapa_bits_area_nodos_i=1;
	sbp.sec_mapa_bits_bloques=6;
	sbp.sec_tabla_nodos_i=3;
	sbp.sec_log_particion=43199;
	sbp.sec_x_bloque=2;
	sbp.heads=HEADS;
	sbp.cyls=CYLINDERS;
	sbp.secfis=SECTORS;
	
	int unidad=0;
	
	int res = vdreadsector(0,0,0,1,1,(char *) &mbr);
	
	// Leer la tabla de particiones, Cilindro 0, Superficie 0, SF 1
	// Determino CHS Inicial
	int cilindro_inicial=
	(((0xC0 & (mbr.partition[0].CHS_begin[1])) >> 30)
		|(mbr.partition[0].CHS_begin[2]));
		
	int superficie_inicial= (mbr.partition[0].CHS_begin[0]);
	
	int secfis_inicial= (mbr.partition[0].CHS_begin[1] & 0x3F);
	
	printf("CHS Inicial %X %X %X ",cilindro_inicial
		   							,superficie_inicial
		   							,secfis_inicial);
		   							
	// Escribir la estructura en el sector lógico 0 de la partición
	vdwritesector(unidad,superficie_inicial,cilindro_inicial,secfis_inicial,1,(char *) &sbp);
}