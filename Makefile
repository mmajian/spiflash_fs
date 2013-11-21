
CC      = gcc
CFLAGS  = -g
RM:= rm -f

PROGRAMS := mytest
all : $(PROGRAMS)

mytest: spiflash_fs.o mytest.o libspiflash_fs.o
	$(CC) $(LDFLAGS) spiflash_fs.o mytest.o libspiflash_fs.o -o $@
spiflash_fs.o: spiflash_fs.c spiflash_fs.h
	$(CC) $(CFLAGS) -c $< -o $@
mytest.o: mytest.c
	$(CC) $(CFLAGS) -c $< -o $@
libspiflash_fs.o:libspiflash_fs.c libspiflash_fs.h
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	$(RM) *.o $(PROGRAMS)
