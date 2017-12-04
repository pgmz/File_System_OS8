
#include "funciones_alto_nivel.h"

int fileTableLoaded = 0;
struct OPENFILES openfiles[24];

extern struct INODE inode[24];

unsigned short *postoptr(int fd,int pos)
{
	int currinode;
	unsigned short *currptr;
	unsigned short indirect1;

	currinode=openfiles[fd].inode;

	// Está en los primeros 10 K
	if((pos/1024)<10)
		// Está entre los 10 apuntadores directos
		currptr=&inode[currinode].direct_blocks[pos/1024];
	else if((pos/1024)<522)
	{
		// Si el indirecto está vacío, asígnale un bloque
		if(inode[currinode].indirect==0)
		{
			// El primer bloque disponible
			indirect1=nextfreeblock();
			assignblock(indirect1); // Asígnalo
			inode[currinode].indirect=indirect1;
		} 
		currptr=&openfiles[fd].buffindirect[pos/1024-10];
	}
	else
		return(NULL);

	return(currptr);
}

unsigned short *currpostoptr(int fd)
{
	unsigned short *currptr;

	currptr=postoptr(fd,openfiles[fd].currpos);

	return(currptr);
}

void loadFileTable(){
	int i;
	if(!fileTableLoaded){
		for(i = 3; i < 24; i++){
			openfiles[i].currbloqueenmemoria = -1;
			openfiles[i].inuse = 0;
		}
		fileTableLoaded = 1;
	}
}

int getFreeFileTable(){
	int i;
	
	for(i = 3; i < 24; i++){
		if(openfiles[i].inuse == 0){
			break;
		}
	}
	
	return (i > 24)?(-1):(i);
}

int vdopen(char *filename,unsigned short mode)
{
	// Les toca hacerla a ustedes
	int numinode = searchinode(filename);
	
	if(numinode == -1){
		return -1;
	}
	
	loadFileTable();
	
	int indexFileTable = getFreeFileTable();
	
	if(indexFileTable == -1){
		return -1;
	}
	
	openfiles[indexFileTable].inuse = 1;
	openfiles[indexFileTable].inode = numinode;
	openfiles[indexFileTable].currpos = 0;
	
	if(inode[numinode].indirect != 0){
		readblock(inode[numinode].indirect, (char *) openfiles[indexFileTable].buffindirect);
	}
	
	unsigned short currblock = *currpostoptr(indexFileTable);
	
	readblock(currblock, openfiles[indexFileTable].buffer);
	openfiles[indexFileTable].currbloqueenmemoria = currblock;
	
	return indexFileTable;
}


// Esta función se va a usar para crear una función en el 
// sistema de archivos
int vdcreat(char *filename,unsigned short perms)
{
	int numinode;
	int i;

	// Ver si ya existe el archivo
	numinode=searchinode(filename);
	if(numinode==-1) // Si el archivo no existe
	{
		// Buscar un inodo en blanco en el mapa de bits (nodos i)
		numinode=nextfreeinode(); // Recorrer la tabla 
									// de nodos i buscando
									// un inodo que esté
									// libre
		if(numinode==-1) // La tabla de nodos-i está llena
		{
			return(-1); // No hay espacio para más archivos
		}
	} else	// Si el archivo ya existe elimina el inodo
		removeinode(numinode);


	// Escribir el archivo en el inodo encontrado
// En un inodo de la tabla, escribe los datos del archivo
	setninode(numinode,filename,perms,getuid(),getgid());
		
	// assigninode(numinode);

	// Poner el archivo en la tabla de archivos abiertos
	// Establecer el archivo como abierto
	loadFileTable();

	// Buscar si hay lugar en la tabla de archivos abiertos
	// Si no hay lugar, regresa -1
	i = getFreeFileTable();

	openfiles[i].inuse=1;	// Poner el archivo en uso
	openfiles[i].inode=numinode;  // Indicar que inodo es el
							// del archivo abierto
	openfiles[i].currpos=0; // Y la posición inicial
	openfiles[i].currbloqueenmemoria - 1;
// del archivo es 0
	return(i);
}


// Borrar un archivo del sistema de archivos
int vdunlink(char *filename)
{
	int numinode;
	int i;

	// Busca el inodo del archivo
	numinode=searchinode(filename);
	if(numinode==-1)
		return(-1); // No existe

	removeinode(numinode);
}

// Mover el puntero del archivo a la posición indicada
// a partir de: el inicio si whence es 0, de la posición 
// actual si whence es 1, o a partir del final si whence es 2
int vdseek(int fd, int offset, int whence)
{
	unsigned short oldblock,newblock;

	// Si no está abierto regresa error
	if(openfiles[fd].inuse==0)
		return(-1);  // Regresar ERROR

	// La función currpostoptr es una función que a partir de 
	// la posición actual del apuntador del archivo, 
// me regresa la dirección de memoria en el nodo i o en 
	// en el bloque de apuntadores que contiene el bloque
	// donde está el puntero del archivo
	oldblock=*currpostoptr(fd);
		
	if(whence==0) // A partir del inicio
	{
		// Si el offset está antes del inicio, o después 
// del final, regresar un error
		if(offset<0||offset>inode[openfiles[fd].inode].size)
			return(-1);
		openfiles[fd].currpos=offset;

	} else if(whence==1) // A partir de la posición actual
	{

		// Validar si no estás quieriendo mover antes del
		// inicio del archivo o después del final		
		if(openfiles[fd].currpos+offset>inode[openfiles[fd].inode].size ||
		   openfiles[fd].currpos+offset<0)
			return(-1);
		openfiles[fd].currpos+=offset;

	} else if(whence==2) // A partir del final
	{
		if(offset>inode[openfiles[fd].inode].size ||
		   openfiles[fd].currpos-offset<0)
			return(-1);
		openfiles[fd].currpos=inode[openfiles[fd].inode].size-offset;
	} else
		return(-1);

	// Verificamos si la nueva posición del puntero es un
	// bloque diferente al que estábamos,si es así hay que
	// cargar ese bloque a memoria
	newblock=*currpostoptr(fd);
	
	// Después de mover el puntero, ahora me cambié a otro
	// bloque?
	if(newblock!=oldblock)
	{
		// Escribir el bloque viejo
		writeblock(oldblock,openfiles[fd].buffer);
		// Leer el bloque nuevo
		readblock(newblock,openfiles[fd].buffer);
		// Indicar el nuevo bloque como bloque actual
		// en la tabla de archivos abiertos
		openfiles[fd].currbloqueenmemoria=newblock;
	}

	// Regresamos la posición actual del archivo
	return(openfiles[fd].currpos);
}

// Esta es la función más difícil, 
int vdwrite(int fd, char *buffer, int bytes)
{
	int currblock;
	int currinode;
	int cont=0;
	int sector;
	int i;
	int result;
	unsigned short *currptr;

	loadFileTable();

	// Si no está abierto, regresa error
	if(openfiles[fd].inuse==0)
		return(-1);

	currinode=openfiles[fd].inode;

	// Copiar byte por byte del buffer que recibo como 
// argumento al buffer del archivo
	while(cont<bytes)
	{
		// Obtener la dirección de donde está el bloque que corresponde
		// a la posición actual
		currptr=currpostoptr(fd);
		if(currptr==NULL)
			return(-1);
	
		// Cuál es el bloque en el que escibiríamos
		currblock=*currptr;

		// Si el bloque está en blanco, dale uno
		if(currblock==0)
		{
			currblock=nextfreeblock();
			// El bloque encontrado ponerlo en donde
			// apunta el apuntador al bloque actual
			*currptr=currblock;
			assignblock(currblock);	// Asignarlo en el mapa de bits
			
			// Escribir el sector de la tabla de nodos i
			// En el disco
			sector=(currinode/8);
			result=vdwriteseclog(getStartOfiNodeArea()+sector,(char *) &inode[sector*8]);
		}

		// Si el bloque de la posición actual no está en memoria
		// Lee el bloque al buffer del archivo
		if(openfiles[fd].currbloqueenmemoria!=currblock)
		{
			// Leer el bloque actual hacia el buffer que
			// está en la tabla de archivos abiertos
			readblock(currblock,openfiles[fd].buffer);			
			// Actualizar en la tabla de archivps abiertos
			// el bloque actual
			openfiles[fd].currbloqueenmemoria=currblock;
		}

		// Copia el caracter al buffer
		openfiles[fd].buffer[openfiles[fd].currpos%TAMBLOQUE]=buffer[cont];

		// Incrementa posición actual del actual
		openfiles[fd].currpos++;

		// Si la posición es mayor que el tamaño, modifica el tamaño
		if(openfiles[fd].currpos>inode[currinode].size)
			inode[openfiles[fd].inode].size=openfiles[fd].currpos;

		// Incrementa el contador
		cont++;

		// Si se llena el buffer, escríbelo
		if(openfiles[fd].currpos%TAMBLOQUE==0)
			writeblock(currblock,openfiles[fd].buffer);
	}
	return(cont);
} 

int vdread(int fd, char *buffer, int bytes)
{
	int currblock;
	int currinode;
	int cont=0;
	int sector;
	int i;
	int result;
	unsigned short *currptr;

	loadFileTable();

	// Si no está abierto, regresa error
	if(openfiles[fd].inuse==0)
		return(-1);

	currinode=openfiles[fd].inode;

	// Copiar byte por byte del buffer que recibo como 
// argumento al buffer del archivo
	while(cont<bytes)
	{
		// Obtener la dirección de donde está el bloque que corresponde
		// a la posición actual
		currptr=currpostoptr(fd);
		if(currptr==NULL)
			return(-1);
	
		// Cuál es el bloque en el que escibiríamos
		currblock=*currptr;

		// Si el bloque está en blanco, dale uno
		if(currblock==0)
		{
			return 0;
		}

		// Si el bloque de la posición actual no está en memoria
		// Lee el bloque al buffer del archivo
		if(openfiles[fd].currbloqueenmemoria!=currblock)
		{
			// Leer el bloque actual hacia el buffer que
			// está en la tabla de archivos abiertos
			readblock(currblock,openfiles[fd].buffer);			
			// Actualizar en la tabla de archivps abiertos
			// el bloque actual
			openfiles[fd].currbloqueenmemoria=currblock;
		}

		// Copia el caracter al buffer
		buffer[cont] = openfiles[fd].buffer[openfiles[fd].currpos%TAMBLOQUE];

		// Incrementa posición actual del actual
		openfiles[fd].currpos++;

		// Incrementa el contador
		cont++;


		// Si la posición es mayor que el tamaño, modifica el tamaño
		if(openfiles[fd].currpos==inode[currinode].size)
			break;
	}
	return(cont);
} 


int vdclose(int fd){
		
	loadFileTable();
	
	if(openfiles[fd].inuse==0)
		return(-1);
	
	//Pendiente, actualizar buffer de indirectos en archivo
	
	openfiles[fd].inuse = 0;
	openfiles[fd].currpos = 0;
	openfiles[fd].currbloqueenmemoria = -1;
	
	memset(openfiles[fd].buffer, 0, 1024);
	memset((char *) openfiles[fd].buffindirect, 0, 512);

}