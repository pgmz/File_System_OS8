
#include "funciones_bajo_nivel.h"

int isblockfree(int block);	
// Usando el mapa de bits, buscar el siguiente bloque libre
int nextfreeblock();
// Asignar un bloque en 1 en el mapa de bits, lo cual significa que estaría ya ocupado.
int assignblock(int block);
// Poner en 0 el bit que corresponde a un bloque en el mapa de bits, esto equivale a decir que el bloque está libre
int unassignblock(int block);