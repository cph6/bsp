CFLAGS = -g -Wall
CCFLAGS = $(CFLAGS)
LFLAGS = 
LIBS = -lm

bsp: bsp.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $< $(LIBS)

bsp.o: bsp.c bsp.h funcs.c makenode.c structs.h

clean:
	rm -f *.o bsp bsp.exe

allclean:
	make clean

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@
