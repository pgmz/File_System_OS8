#include "funciones_alto_nivel_dir.h"

VDDIR dirs[2]={-1,-1};
struct vddirent current;

extern int secboot_en_memoria;
extern struct SECBOOTPART secboot;

extern int nodos_i_en_memoria;
extern struct INODE inode[24];

VDDIR *vdopendir(char *path)
{
	int i=0;
	int result;

	if(!secboot_en_memoria)
	{
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}

//Aquí se debe calcular la variable inicio_nodos_i con los datos que están en el sector de boot de la partición

	// Determinar si la tabla de nodos i está en memoria
// si no está en memoria, hay que cargarlos
	if(!nodos_i_en_memoria)
	{
		for(i=0;i<secboot.sec_tabla_nodos_i;i++)
			result=vdreadseclog(getStartOfiNodeArea()+i,(char *) &inode[i*8]);

		nodos_i_en_memoria=1;
	}

	if(strcmp(path,".")!=0)
		return(NULL);

	i=0;
	while(dirs[i]!=-1 && i<2)
		i++;

	if(i==2)
		return(NULL);

	dirs[i]=0;

	return(&dirs[i]);	
}


// Lee la siguiente entrada del directorio abierto
struct vddirent *vdreaddir(VDDIR *dirdesc)
{
	int i;

	int result;
	if(!nodos_i_en_memoria)
	{
		for(i=0;i<secboot.sec_tabla_nodos_i;i++)
			result=vdreadseclog(getStartOfiNodeArea()+i,(char *) &inode[i*8]);

		nodos_i_en_memoria=1;
	}

	// Mientras no haya nodo i, avanza
	while(isinodefree(*dirdesc) && *dirdesc<4096)
		(*dirdesc)++;

	// Apunta a donde está el nombre en el inodo	
	current.d_name=inode[*dirdesc].name;

	(*dirdesc)++;

	if(*dirdesc>=24)
		return(NULL);
	return( &current);	
}



int vdclosedir(VDDIR *dirdesc)
{
	(*dirdesc)=-1;
}