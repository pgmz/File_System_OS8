/* Este programa escribe el MBR que contiene la tabla de particiones
   en el disco */

#include <stdio.h>
#include <string.h>
#include "vdisk.h"

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

// Debe medir 64 bytes, importante es que el tamaño sea potencia de 2
struct INODE {
	char name[18];
	unsigned int datetimecreat;	// 32 bits
	unsigned int datetimemodif;	// 32 bits
	unsigned int datetimelaacc; // 32 bits
	unsigned short uid;		// 16 bits
	unsigned short gid;		// 16 bits
	unsigned short perms;	// 16 bits
	unsigned int size;			// 32 bits
	unsigned short direct_blocks[10];	// 10 x 16 bits = 20 bytes
	unsigned short indirect;	// 16 bits
	unsigned short indirect2;	// 16 bits
};
// printf("%d\n",sizeof(struct INODE));

int main()
{
	struct MBR mbr;
	
	int cilinic=0,supinic=0,sfinic=2;
	int cilfin=599,supfin=5,sffin=17;
	
	memset(&mbr,0,512);
	
	mbr.partition[0].CHS_begin[0]=(unsigned char) supinic;
	mbr.partition[0].CHS_begin[1]=(unsigned char) sfinic & 0x3F;
	mbr.partition[0].CHS_begin[2]=(unsigned char) (cilinic & 0xFF);
	mbr.partition[0].CHS_begin[1]=mbr.partition[0].CHS_begin[1] |
									(unsigned char) ((cilinic & 0x300)>>2);
	
	mbr.partition[0].CHS_end[0]=(unsigned char) supfin;
	mbr.partition[0].CHS_end[1]=(unsigned char) sffin & 0x3F;
	mbr.partition[0].CHS_end[2]=(unsigned char) (cilfin & 0xFF);
	mbr.partition[0].CHS_end[1]=mbr.partition[0].CHS_end[1] |
									(unsigned char) ((cilfin & 0x300)>>2);
	
	printf("CHS Inicial %X %X %X ",mbr.partition[0].CHS_begin[2]
		   							,mbr.partition[0].CHS_begin[1]
		   							,mbr.partition[0].CHS_begin[0]);
	printf("CHS Final %X %X %X\n",mbr.partition[0].CHS_end[2]
		   							,mbr.partition[0].CHS_end[1]
		   							,mbr.partition[0].CHS_end[0]);
	
	vdwritesector(0,0,0,1,1,(char *) &mbr);

}