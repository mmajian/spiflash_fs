
CC      = gcc
CFLAGS  = -g
RM:= rm -f

PROGRAMS := mytest
all : $(PROGRAMS)

mytest: spiflash_fs.o mytest.o
	$(CC) $(LDFLAGS) spiflash_fs.o mytest.o -o $@

spiflash_fs.o: spiflash_fs.c
	$(CC) $(CFLAGS) -c $< -o $@
mytest.o: mytest.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	$(RM) *.o $(PROGRAMS)
