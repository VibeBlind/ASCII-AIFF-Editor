/* Author: Seth George */

#include "sndlib.h"

/**
 This program converts from CS229 file format AIFF, and vice versa.
 The program reads in the name of the input file, and then read in the name of the output file
 The output file should be written in the opposing file format
*/
void main(){
	soundfile_t from;
	char name[MAX_NAME_LENGTH];
	char format[6];
	
	printf("Enter the pathname of the sound file you want to convert:\n");
	fgets(from.name, MAX_NAME_LENGTH, stdin); /* Take in name of read from file */
	from.fp = fopen(from.name, "r"); /* open file as read only */
	
	printf("Enter the pathname you wish to name the new sound file:\n");
	fgets(name, MAX_NAME_LENGTH, stdin); /* Take in name of write to file */

	void setFormat(soundfile_t *from); /* should throw error if not CS229 or AIFF */
	
	FILE *to = fopen(name, "w"); /* make file of opposing format */
	if(strcmp(from.format, "CS229") == 0){
		parseCs229(&from);
		/* writeAiff(&from, &to); */
	} else {
		parseAiff(&from);
		/* writeCs229(&from, &to);*/
	}
	
	fclose(from.fp);
}