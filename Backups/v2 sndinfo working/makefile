all:sndinfo sndconv

sndinfo: sndinfo.o sndutil.o
	gcc -o sndinfo sndinfo.o sndutil.o
sndinfo.o: sndinfo.c
	gcc -c sndinfo.c
sndutil.o: sndutil.c sndlib.h
	gcc -c sndutil.c

sndconv: sndconv.o sndutil.o
	gcc -o sndconv sndconv.o sndutil.o
sndconv.o: sndconv.c
	gcc -c sndconv.c

	
clean:
	rm -f *.o sndinfo sndconv