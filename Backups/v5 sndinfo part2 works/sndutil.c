/* Author: Seth George */

#include "sndutil.h"

/**
*	Set all int variables in struct to zero
*/
void initSoundfile(soundfile_t *snd){
	snd->samples = -1;
	snd->sampleRate = 0;
	snd->bitDepth = 0;
	snd->channels = 0;
	snd->duration = 0;
	snd->numBytes = 0;
}

/**
*	Check if sample rate, bit depth, and channels are set
*/
void keywordsAreSet(soundfile_t *snd){
	if((snd->sampleRate == 0)||(snd->bitDepth == 0)||(snd->channels == 0)){
		fprintf(stderr, "Error: Missing required keyword\n");
		exit(EXIT_FAILURE);
	}
}

/**
*	Exponential function
*/
int power(int num, int exp){
	int power = 1;
	while(exp){
		power *= num;
		exp--;
	}
	return power;
}

/**
*	Print out the parsed file info
*	Print information about the sound file
*/
void printSndInfo(soundfile_t *snd){
	float secs = snd->duration;
	int hrs;
	int mins;
	
	/* Duration is broken up into hrs:mins:secs:fracs  */
	hrs = secs / 3600;
	secs -= hrs * 3600;
	mins = secs / 60;
	secs -= mins * 60;

	printf("----------------------------------------------------------------------\n");
	printf("Filename: %s\n", snd->name);
	printf("Format: %s\n", snd->format);
	printf("SampleRate: %d\n", snd->sampleRate);
	printf("BitDepth: %d\n", snd->bitDepth);
	printf("Channels: %d\n", snd->channels);
	printf("Samples: %d\n", snd->samples);
	printf("Duration: %d:%d:%.2f\n", hrs, mins, secs);
}

/**
*	Determines file format
*/
void setFormat(soundfile_t *snd){
	if(!snd->stream){ /* invalid file pointer */
		fprintf(stderr, "Error: no such file\n");
		exit(EXIT_FAILURE);
	} else { /* valid file pointer */
		char test[5];
		fgets(test, 5, snd->stream);
		/* compare the first 4 bytes */
		if(strncmp("FORM", test, 4) == 0){ 
			strncpy(snd->format, "AIFF", 6);
			snd->format[5] = 0;
		} else if(strncmp("CS22", test, 4) == 0){
			if(fgetc(snd->stream) == '9'){ /* check if the next byte is 9 */
				strncpy(snd->format, "CS229", 6);
			} else {
				fprintf(stdout, "Error: Invalid File Format\n");
				exit(EXIT_FAILURE);
			}
		} else {
			fprintf(stdout, "Error: Invalid File Format\n");
			exit(EXIT_FAILURE);
		}
	}
}

/**
*	Sndinfo as specified in part 1
*/
void basicSndinfo(){
	soundfile_t snd;
	initSoundfile(&snd);
	
	printf("Enter the pathname of a sound file:\n");
	fgets(snd.name, MAX_NAME_LENGTH, stdin); /* take in name from standard input */
	
	if(snd.name[strlen(snd.name) -1] == '\n'){
		snd.name[strlen(snd.name) -1] = '\0';
	}
	snd.stream = fopen(snd.name, "r"); /* open file as read only */
	
	if(!snd.stream){ /* invalid file pointer */
		fprintf(stderr, "Error: no such file\n");
		exit(EXIT_FAILURE);
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
	printf("----------------------------------------------------------------------\n");
	exit(EXIT_SUCCESS);
}

/**
*	Sndconv as specified in part 1
*/
void basicSndconv(){
	soundfile_t from;
	initSoundfile(&from);
	
	printf("Enter the pathname of the sound file you want to convert:\n");
	fgets(from.name, MAX_NAME_LENGTH, stdin); /* Take in name of read from file */
	if(from.name[strlen(from.name) -1] == '\n'){
		from.name[strlen(from.name) -1] = '\0';
	}
	from.stream = fopen(from.name, "r"); /* open file as read only */
	if(!from.stream){ /* invalid file pointer */
		fprintf(stderr, "Error: no such file\n");
		exit(1);
	
	}
	setFormat(&from); /* should throw error if not CS229 or AIFF */

	if(strcmp(from.format, "CS229") == 0){
		parseCs229(&from);
	} else {
		parseAiff(&from);
	}
	fclose(from.stream);
	
	char name[MAX_NAME_LENGTH];
	printf("Enter the pathname you wish to name the new sound file:\n");
	fgets(name, MAX_NAME_LENGTH, stdin); /* Take in name of write to file */
	if(name[strlen(name) -1] == '\n'){
		name[strlen(name) -1] = '\0';
	}
	
	FILE* to = fopen(name, "w"); /* make file of opposing format */
	if(!to){ /* invalid file pointer */
		fprintf(stderr, "Error: could not open file for writing\n");
		exit(EXIT_FAILURE);
	}
	if(strcmp(from.format, "CS229") == 0){
		writeAiff(&from, to);
	} else {
		writeCs229(&from, to);
	}
	
	fclose(to);
}