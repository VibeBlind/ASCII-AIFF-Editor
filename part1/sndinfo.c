/* Author: Seth George */

#include "sndlib.h"

void main(){
	soundfile_t snd;
	initSoundfile(&snd);
	
	printf("Enter the pathname of a sound file:\n");
	fgets(snd.name, MAX_NAME_LENGTH, stdin); /* take in name from standard input */
	
	if(snd.name[strlen(snd.name) -1] == '\n'){
		snd.name[strlen(snd.name) -1] = '\0';
	}
	snd.fp = fopen(snd.name, "r"); /* open file as read only */
	
	if(!snd.fp){ /* invalid file pointer */
		fprintf(stderr, "Error: no such file\n");
		exit(1);
	
	} else { /* valid file pointer */
		setFormat(&snd);
		if(strcmp(snd.format, "CS229") == 0){
			parseCs229(&snd);
		} else {
			parseAiff(&snd);
		}
		
		fclose(snd.fp);/* close file after parsing */
		snd.duration = ((float)snd.samples)/ ((float)snd.sampleRate); /* duration in seconds */
		
		printSndInfo(&snd);
	}
}