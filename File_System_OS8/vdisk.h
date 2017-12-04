#include <stdio.h>
#include <string.h>

#define HEADS 8
#define SECTORS 27
#define CYLINDERS 200 

#pragma once
#pragma pack(2)

struct PARTITION 
{
	unsigned char status;
	unsigned char CHS_begin[3];
	unsigned char partition_type;
	unsigned char CHS_end[3];
	unsigned int LBA;
	unsigned int secs_per_partition;
};

struct MBR 
{
	unsigned char bootcode[446];	// 446 +
	struct PARTITION partition[4];	//  64 +
	unsigned short bootsignature;	//   2 = 512
};

// Debe medir 512 bytes
struct SECBOOTPART {
	char jump[4];			
	char nombre_particion[8];
	// Tabla de parámetros del bios
	// Están los datos sobre el formato de la partición
	unsigned short sec_inicpart;		// 1 sector 
	unsigned char sec_res;		// 1 sector reservado para el sector de boot de la partición
	unsigned char sec_mapa_bits_area_nodos_i;// 1 sector
	unsigned char sec_mapa_bits_bloques;	// 6 sectores
	unsigned short sec_tabla_nodos_i;	// 3 sectores
	unsigned int sec_log_particion;		// 43199 sectores
	unsigned char sec_x_bloque;			// 2 sectores por bloque
	unsigned char heads;				// 8 superficies				
	unsigned char cyls;				// 200 cilindros
	unsigned char secfis;				// 27 sectores por track
	char restante[484];	// Código de arranque
};
// printf("%d\n",sizeof(struct SECBOOT));

int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);