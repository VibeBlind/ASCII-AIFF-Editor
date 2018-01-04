/* Author: Seth George */

#include "sndlib.h"

/**
 This program converts from CS229 file format AIFF, and vice versa.
 The program reads in the name of the input file, and then read in the name of the output file
 The output file should be written in the opposing file format
*/
int main(int argc, char *argv[]){
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
		exit(1);
	}
	if(strcmp(from.format, "CS229") == 0){
		writeAiff(&from, to);
	} else {
		writeCs229(&from, to);
	}
	
	fclose(to);
	return 0;
}