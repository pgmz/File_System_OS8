
#include "funciones_alto_nivel.h"
#include "funciones_alto_nivel_dir.h"
/*
typedef int VDDIR;
	
struct vddirent 
{
	char *d_name;
};

struct vddirent *vdreaddir(VDDIR *dirdesc);
VDDIR *vdopendir(char *path);
VDDIR *vdclosedir(VDDIR *path);
*/
void locateend(char *linea);
int executecmd(char *linea);
int isinvd(char *arg);
int copyuu(char *arg1,char *arg2);
int copyuv(char *arg1,char *arg2);
int copyvu(char *arg1,char *arg2);
int copyvv(char *arg1,char *arg2);
int catv(char *arg1);
int catu(char *arg1);
int diru(char *arg1);
int dirv(char *dir);