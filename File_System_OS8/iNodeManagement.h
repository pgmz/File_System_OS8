// *************************************************************************
// Para el mapa de bits del área de nodos i
// *************************************************************************

#include <stdio.h>
#include <string.h>
#include "DateManagement.h"
#include "BlockManagement.h"

#define TOTAL_NODOS_I		24 //tenemos 3 sectores para la tabla de inodos, (512 * 3), y cada estructura de inodo, mide 64, por esto
								// (512 * 3)/(64) = 24

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

int getStartOfiNodeArea();
int getStartOfiNodeMap();
// Usando el mapa de bits, determinar si un nodo i, está libre u ocupado.
int isinodefree(int inode);
// Buscar en el mapa de bits, el primer nodo i que esté libre, es decir, que su bit está en 0, me regresa ese dato
int nextfreeinode();
// Poner en 1, el bit que corresponde al número de inodo indicado
int assigninode(int inode);
// Poner en 0, el bit que corresponde al número de inodo indicado
int unassigninode(int inode);
// Escribir en la tabla de nodos-I del directorio raíz, los datos de un archivo
int setninode(int num, char *filename,unsigned short atribs, int uid, int gid);
// Buscar en la tabla de nodos I, el nodo I que contiene el 
// nombre del archivo indicado en el apuntador a la cadena
// Regresa el número de nodo i encontrado
// Si no lo encuentra, regresa -1
int searchinode(char *filename);
// Eliminar un nodo I de la tabla de nodos I, y establecerlo 
// como disponible
int removeinode(int numinode);
