// *************************************************************************
// Para el mapa de bits del área de nodos i
// *************************************************************************

#include "iNodeManagement.h"

int mapa_bits_nodos_i;

extern int secboot_en_memoria;
extern struct SECBOOTPART secboot;

int inodesmap_en_memoria = 0;
//512, porque eso mide un
char inodesmap[512];

int nodos_i_en_memoria = 0;
struct INODE inode[24];

int getStartOfiNodeArea(){
	
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
	
	return secboot.sec_mapa_bits_area_nodos_i + secboot.sec_mapa_bits_bloques + 1;
}

int getStartOfiNodeMap(){
	
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
	
	return secboot.sec_mapa_bits_area_nodos_i;
}

// Usando el mapa de bits, determinar si un nodo i, está libre u ocupado.
int isinodefree(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;

	// Checar si el sector de boot de la partición está en memoria
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}
	
	mapa_bits_nodos_i= secboot.sec_inicpart +secboot.sec_res; 	
//Usamos la información del sector de boot de la partición para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(mapa_bits_nodos_i,(char *) &inodesmap);
		inodesmap_en_memoria=1;
	}


	if(inodesmap[offset] & (1<<shift))
		return(0); // El nodo i ya está ocupado
	else
		return(1); // El nodo i está libre
}

// Buscar en el mapa de bits, el primer nodo i que esté libre, es decir, que su bit está en 0, me regresa ese dato
int nextfreeinode()
{
	int i,j;
	int result;

	// Checar si el sector de boot de la partición está en memoria
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}
	mapa_bits_nodos_i= secboot.sec_inicpart +secboot.sec_res; 	
//Usamos la información del sector de boot de la partición para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(mapa_bits_nodos_i,(char *) &inodesmap);
		inodesmap_en_memoria=1;
	}

	// Recorrer byte por byte mientras sea 0xFF sigo recorriendo
	i=0;
	while(inodesmap[i]==0xFF && i<3)
		i++;

	if(i<3)
	{
		// Recorrer los bits del byte, para encontrar el bit
		// que está en cero
		j=0;
		while(inodesmap[i] & (1<<j) && j<8)
			j++;

		return(i*8+j); // Regresar el bit del mapa encontrado en cero
	}
	else // Todos los bits del mapa de nodos i están en 1
		return(-1); // -1 significa que no hay nodos i disponibles

		
}


// Poner en 1, el bit que corresponde al número de inodo indicado
int assigninode(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;

	// Checar si el sector de boot de la partición está en memoria
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}
	mapa_bits_nodos_i= secboot.sec_inicpart +secboot.sec_res; 	
//Usamos la información del sector de boot de la partición para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(mapa_bits_nodos_i,(char *) &inodesmap);
		inodesmap_en_memoria=1;
	}

	inodesmap[offset]|=(1<<shift); // Poner en 1 el bit indicado
	vdwriteseclog(mapa_bits_nodos_i,(char *) &inodesmap);
	return(1);
}

// Poner en 0, el bit que corresponde al número de inodo indicado
int unassigninode(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;

	// Checar si el sector de boot de la partición está en memoria
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}
	mapa_bits_nodos_i= secboot.sec_inicpart +secboot.sec_res; 	
//Usamos la información del sector de boot de la partición para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(mapa_bits_nodos_i,(char *) &inodesmap);
		inodesmap_en_memoria=1;
	}


	inodesmap[offset]&=(char) ~(1<<shift); // Poner en cero el bit que corresponde al inodo indicado
	vdwriteseclog(mapa_bits_nodos_i,(char *) &inodesmap);
	return(1);
}

// Escribir en la tabla de nodos-I del directorio raíz, los datos de un archivo
int setninode(int num, char *filename,unsigned short atribs, int uid, int gid)
{
	int i;
	int result;

/*
	Antes de continuar debe cargarse en memoria el sector de boot de la partición.
	Con los datos que están ahí, calcular:
El sector lógico donde empieza la tabla de nodos-i del directorio raiz.
También vamos a usar el número de sectores que tiene la tabla de nodos-i
*/

	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}

	//Usamos la información del sector de boot de la partición para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 
	mapa_bits_nodos_i= secboot.sec_inicpart +secboot.sec_res; 	
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(mapa_bits_nodos_i,(char *) &inodesmap);
		inodesmap_en_memoria=1;
	}
	
	// Si la tabla de nodos-i no está en memoria, 
// hay que cargarla a memoria
	if(!nodos_i_en_memoria)
	{
		for(i=0;i<secboot.sec_tabla_nodos_i;i++)
			result=vdreadseclog(getStartOfiNodeArea()+i, (char *) &inode[i*8]);

		nodos_i_en_memoria=1;
	}

	// Copiar el nombre del archivo en el nodo i
	strncpy(inode[num].name,filename,18);
	// Asegurando que el último caracter es el terminador (cero)
	if(strlen(inode[num].name)>17)
	 	inode[num].name[17]='\0';

	// Poner en el nodo I las fechas y horas de creación
	// con las fecha y hora actual
	inode[num].datetimecreat=currdatetimetoint();
	inode[num].datetimemodif=currdatetimetoint();
	// Información sobre el dueño, grupo dueño y atributos
	// Para propósitos de la práctica, los datos que se 
	// refieren a dueño, grupo dueño y atributos (permisos)
	// no son relevantes.
	inode[num].uid=uid;
	inode[num].gid=gid;
	inode[num].perms=atribs;

	// Un archivo nuevo, su tamaño inicial es 0
	inode[num].size=0;	// Tamaño del archivo en 0
	
	// Establecer los apuntadores a bloques directos en 0
	for(i=0;i<10;i++)
		inode[num].direct_blocks[i]=0;

	// Establecer los apuntadores indirectos en 0
	inode[num].indirect=0;
	inode[num].indirect2=0;

	// Optimizar la escritura escribiendo solo el sector lógico que
	// corresponde al inodo que estamos asignando.
	// i=num/8;
	// result=vdwriteseclog(inicio_nodos_i+i,&inode[i*8]);
	i = num/8;
	result=vdwriteseclog(getStartOfiNodeArea() + i, (char *) &inode[i*8]);

	return(num);
}

// Buscar en la tabla de nodos I, el nodo I que contiene el 
// nombre del archivo indicado en el apuntador a la cadena
// Regresa el número de nodo i encontrado
// Si no lo encuentra, regresa -1
int searchinode(char *filename)
{
	int i;
	int free;
	int result;

/*
Antes de continuar debe cargarse en memoria el sector de boot de la partición.

Con los datos que están ahí, calcular:
El sector lógico donde empieza la tabla de nodos-i
También vamos a usar el número de sectores que tiene la tabla de nodos-i
*/

	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}
	

	//Usamos la información del sector de boot de la partición para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 
	mapa_bits_nodos_i= secboot.sec_inicpart +secboot.sec_res; 	
					
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(mapa_bits_nodos_i,(char *) &inodesmap);
		inodesmap_en_memoria=1;
	}

	// Si la tabla de nodos i no está en memoria, 
// entonces vamos a traer los sectores lógicos de 
// los nodos I a memoria
	if(!nodos_i_en_memoria)
	{
		for(i=0;i<secboot.sec_tabla_nodos_i;i++)
			result=vdreadseclog(getStartOfiNodeArea()+i, (char *) &inode[i*8]);

		nodos_i_en_memoria=1;
	}
	
	// El nombre del archivo no debe medir más de 18 bytes
	if(strlen(filename)>17)
	  	filename[17]='\0';

	// Recorrer la tabla de nodos I que ya tengo en memoria
// desde el principio hasta el final buscando el archivo.


	i=0;
		printf("%s \n", inode[i].name);
	while(strcmp(inode[i].name,filename) && i<TOTAL_NODOS_I)
		i++;

	if(i>= TOTAL_NODOS_I)
		return(-1);		// No se encuentra el archivo
	else
		return(i);		// La posición donde fue encontrado 
}

// Eliminar un nodo I de la tabla de nodos I, y establecerlo 
// como disponible
int removeinode(int numinode)
{
	int i;
	int result;
	unsigned short temp[512]; // 1024 bytes
// Desasignar los bloques directos en el mapa de bits que 
// corresponden al archivo

/*
Antes de continuar debe cargarse en memoria el sector lógico 1 que es el sector de boot de la partición.

Con los datos que están ahí, calcular:
El sector lógico donde empieza la tabla de nodos-i
También vamos a usar el número de sectores que tiene la tabla de nodos-i

Asegurar que los sectores de la tabla nodos-I están en memoria, si no están en memoria, cargarlos.
*/
if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}
	

	//Usamos la información del sector de boot de la partición para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 
	mapa_bits_nodos_i= secboot.sec_inicpart +secboot.sec_res; 	
					
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(mapa_bits_nodos_i,(char *) &inodesmap);
		inodesmap_en_memoria=1;
	}

	// Si la tabla de nodos i no está en memoria, 
// entonces vamos a traer los sectores lógicos de 
// los nodos I a memoria
	if(!nodos_i_en_memoria)
	{
		for(i=0;i<secboot.sec_tabla_nodos_i;i++)
			result=vdreadseclog(getStartOfiNodeArea()+i, (char *) &inode[i*8]);

		nodos_i_en_memoria=1;
	}


// Recorrer los apuntadores directos del nodo i
	for(i=0;i<10;i++)
		if(inode[numinode].direct_blocks[i]!=0) // Si es dif de cero
										// Si está asignado
		{
			unassignblock(inode[numinode].direct_blocks[i]);
			inode[numinode].direct_blocks[i]=0;
		}

	// Si el bloque indirecto, ya está asignado
	if(inode[numinode].indirect!=0)
	{
		// Leer el bloque que contiene los apuntadores
// a memoria
		readblock(inode[numinode].indirect,(char *) temp);
		// Recorrer todos los apuntadores del bloque para
		// desasignarlos 
		for(i=0;i<512;i++)
			if(temp[i]!=0)
				unassignblock(temp[i]);
		// Desasignar el bloque que contiene los apuntadores
		unassignblock(inode[numinode].indirect);
		inode[numinode].indirect=0;
	}

	// Poner en cero el bit que corresponde al inodo en el mapa
	// de bits de nodos-i
	unassigninode(numinode);
	return(1);
}
