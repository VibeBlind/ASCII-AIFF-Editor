/* Author: Seth George */

#include "sndlib.h"

/**
 read first word of file
 check if it is CS229 or FORM
 if neither throw an error
 otherwise parse the file according to the correct format
 
 if CS229 parse each line and look for keywords
 collect the values after each keyword disregarding lines starting with #
 if a line doesn't start with # or a keywords, throw an error and exit
 no keywords after start data
 channels are read on the same line
 
 if AIFF parse each the next 4 bytes to get remaining bytes in file
 search for the COMM chunk and gather keyword information
 search for the SSND chunk and gather it block information
 parse the rest of the file
*/

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
			parseHeader(&snd);
		} else {
			parseAiff(&snd);
		}
		
		fclose(snd.fp);/* close file after parsing */
		snd.duration = ((float)snd.samples)/ ((float)snd.sampleRate); /* duration in seconds */
		
		printSndInfo(&snd);
	}
}