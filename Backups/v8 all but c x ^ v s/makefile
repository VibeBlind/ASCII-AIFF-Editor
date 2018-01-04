all: sndinfo sndconv sndcat sndcut sndshow sndedit

clean:
	rm -f *.o tarball sndinfo sndconv sndcat sndcut sndshow sndedit 
	
tarball: seth_george_proj1_part2.tar.gz

seth_george_proj1_part2.tar.gz: makefile sndinfo.c sndconv.c sndcat.c sndcut.c sndshow.c sndedit.c sndutil.c cs229util.c aiffutil.c editutil.c sndutil.h cs229util.h aiffutil.h editutil.h README.txt
	tar -czf seth_george_proj1_part2.tar.gz makefile sndinfo.c sndconv.c sndcat.c sndcut.c sndshow.c sndedit.c sndutil.c cs229util.c aiffutil.c editutil.c sndutil.h cs229util.h aiffutil.h editutil.h README.txt

sndinfo: sndinfo.o sndutil.o cs229util.o aiffutil.o
	gcc -o sndinfo sndinfo.o sndutil.o cs229util.o aiffutil.o
sndconv: sndconv.o sndutil.o cs229util.o aiffutil.o
	gcc -o sndconv sndconv.o sndutil.o cs229util.o aiffutil.o
sndcat: sndcat.o sndutil.o cs229util.o aiffutil.o
	gcc -o sndcat sndcat.o sndutil.o cs229util.o aiffutil.o
sndcut: sndcut.o sndutil.o cs229util.o aiffutil.o
	gcc -o sndcut sndcut.o sndutil.o cs229util.o aiffutil.o
sndshow: sndshow.o sndutil.o cs229util.o aiffutil.o
	gcc -o sndshow sndshow.o sndutil.o cs229util.o aiffutil.o
sndedit: sndedit.o sndutil.o cs229util.o aiffutil.o editutil.o
	gcc -o sndedit sndedit.o sndutil.o cs229util.o aiffutil.o editutil.o -lncurses
	
sndinfo.o: sndinfo.c cs229util.h aiffutil.h
	gcc -c sndinfo.c
sndconv.o: sndconv.c cs229util.h aiffutil.h
	gcc -c sndconv.c
sndcat.o: sndcat.c cs229util.h aiffutil.h
	gcc -c sndcat.c
sndcut.o: sndcut.c cs229util.h aiffutil.h
	gcc -c sndcut.c
sndshow.o: sndshow.c cs229util.h aiffutil.h
	gcc -c sndshow.c
sndedit.o: sndedit.c cs229util.h aiffutil.h 
	gcc -c sndedit.c
cs229util.o: cs229util.c cs229util.h sndutil.h 
	gcc -c cs229util.c
aiffutil.o: aiffutil.c aiffutil.h sndutil.h 
	gcc -c aiffutil.c
editutil.o: editutil.c editutil.h sndutil.h
	gcc -c editutil.c
sndutil.o: sndutil.c sndutil.h
	gcc -c sndutil.c	
