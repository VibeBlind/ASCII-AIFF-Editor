/* Author: Seth George */

#include "sndlib.h"

int main(int argc, char *argv[]){
	soundfile_t snd;
	initSoundfile(&snd);
	snd.stream = fopen(argv[1], "r"); /* argv[1] should be a file name to open */

	if(!snd.stream){ /* invalid file pointer */
		fprintf(stderr, "Error: no such file\n");
		exit(1);
	}
	
	printf("Enter the pathname of a sound file:\n");
	fgets(snd.name, MAX_NAME_LENGTH, stdin); /* take in name from standard input */
	
	if(snd.name[strlen(snd.name) -1] == '\n'){
		snd.name[strlen(snd.name) -1] = '\0';
	}
	snd.stream = fopen(snd.name, "r"); /* open file as read only */
	
	if(!snd.stream){ /* invalid file pointer */
		fprintf(stderr, "Error: no such file\n");
		exit(1);
	}
	/* valid file pointer */
	setFormat(&snd);
	if(strcmp(snd.format, "CS229") == 0){
		parseCs229(&snd);
	} else {
		parseAiff(&snd);
	}
	
	fclose(snd.stream); /* close file after parsing */
	snd.duration = ((float)snd.samples)/snd.sampleRate; /* duration in seconds */
	
	printSndInfo(&snd);
	
	return 0;
}