all: sndinfo sndconv

clean:
	rm -f *.o tarball sndinfo sndconv
	
tarball: seth_george_proj1_part2.tar.gz

seth_george_proj1_part2.tar.gz: makefile sndinfo.c sndconv.c sndutil.c cs229util.c aiffutil.c sndutil.h cs229util.h aiffutil.h README.txt
	tar -czf seth_george_proj1_part2.tar.gz makefile sndinfo.c sndconv.c sndutil.c cs229util.c aiffutil.c sndutil.h cs229util.h aiffutil.h README.txt

# targets
sndinfo: sndinfo.o sndutil.o cs229util.o aiffutil.o
	gcc -o sndinfo sndinfo.o sndutil.o cs229util.o aiffutil.o
sndconv: sndconv.o sndutil.o cs229util.o aiffutil.o
	gcc -o sndconv sndconv.o sndutil.o cs229util.o aiffutil.o

# dependencies
sndinfo.o: sndinfo.c cs229util.h aiffutil.h
	gcc -c sndinfo.c
sndconv.o: sndconv.c cs229util.h aiffutil.h
	gcc -c sndconv.c
cs229util.o: cs229util.c cs229util.h sndutil.h 
	gcc -c cs229util.c
aiffutil.o: aiffutil.c aiffutil.h sndutil.h 
	gcc -c aiffutil.c
sndutil.o: sndutil.c sndutil.h
	gcc -c sndutil.c