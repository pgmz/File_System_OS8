
#include "iNodeManagement.h"

#define TAMBLOQUE 24

struct OPENFILES {
	int inuse;		// 0 cerrado, 1 abierto
	unsigned short inode;
	int currpos;
	int currbloqueenmemoria;
	char buffer[1024];
	unsigned short buffindirect[512]; //		
};

int vdopen(char *filename,unsigned short mode);
// Esta función se va a usar para crear una función en el 
// sistema de archivos
int vdcreat(char *filename,unsigned short perms);
// Borrar un archivo del sistema de archivos
int vdunlink(char *filename);
// Mover el puntero del archivo a la posición indicada
// a partir de: el inicio si whence es 0, de la posición 
// actual si whence es 1, o a partir del final si whence es 2
int vdseek(int fd, int offset, int whence);
// Esta es la función más difícil, 
int vdwrite(int fd, char *buffer, int bytes);
int vdread(int fd, char *buffer, int bytes);
int vdclose(int fd);
