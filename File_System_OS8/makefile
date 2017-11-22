all: vdfdisk vdformat funciones1 dumpsec 

vdfdisk: vdisk.o vdisk.h funciones1.c
	gcc -o vdfdisk vdfdisk.c -fPIC


dumpsec: vdisk.h
	gcc -o dumpsec dumpsec.c vdisk.h


vdformat: vdisk.h
	gcc -o vdformat vdformat.c 


funciones1: vdisk.o vdisk.h
	gcc -c funciones1.c -fPIC

clean: 
	rm vdfdisk *.o