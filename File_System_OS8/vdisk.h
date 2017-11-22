#define HEADS 8
#define SECTORS 27
#define CYLINDERS 200 
#define SFIP 2
#define HIP 0
#define SIP 0
#define SEC_X_TRACK 27

int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int obtenerSectorFisico(int seclog);
int obtenerCilindro(int seclog);
int obtenerSuperficie(int seclog);