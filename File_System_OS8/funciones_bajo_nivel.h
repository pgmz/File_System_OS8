#include <stdio.h>
#include <string.h>
#include "vdisk.h"

#define SFIP			2
#define HIP				0
#define CIP				0

#define SEC_X_TRACK		27

int vdwriteseclog(int seclog,char *buffer);
int vdreadseclog(int seclog,char *buffer);
int writeblock(int nblock,char *buffer);
int readblock(int nblock,char *buffer);
int getStartOfDataMap();
int getStartOfDataArea();