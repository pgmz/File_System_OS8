#include "iNodeManagement.h"

typedef int VDDIR;

struct vddirent 
{
	char *d_name;
};

VDDIR *vdopendir(char *path);
// Lee la siguiente entrada del directorio abierto
struct vddirent *vdreaddir(VDDIR *dirdesc);
int vdclosedir(VDDIR *dirdesc);