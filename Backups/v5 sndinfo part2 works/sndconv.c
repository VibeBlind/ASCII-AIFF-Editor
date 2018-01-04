/* Author: Seth George */

#include "aiffutil.h"
#include "cs229util.h"

/**
 This program converts from CS229 file format AIFF, and vice versa.
 The program reads in the name of the input file, and then read in the name of the output file
 The output file should be written in the opposing file format
*/
int main(int argc, char *argv[]){
	int forceAIFF = 0; /* flag for force aiff */
	int forceCS229 = 0; /* flag for force cs229 */
	int i;
	for(i = 1; i < argc; i++){
		if(strcmp(argv[i], "-h") == 0){ /* if the arg is -h, display help screen*/
			fprintf(stdout, "Sndconv is read from standard input and writes to standard output.\n");
			fprintf(stdout, "I.e., the standard input stream should be treated as the input file,\n");
			fprintf(stdout, "and the converted file should be written directly to the standard output stream.\n");
			fprintf(stdout, "Any messages (e.g., errors) are be written to standard error.\n");
			fprintf(stdout, "This program supports the following switches, passed as arguments.\n");
			fprintf(stdout, "\t-h: Displays a short help screen to standard error, and then terminates cleanly\n");
			fprintf(stdout, "\t-1: Prompts for a file name rather than accepting it as an argument\n");
			fprintf(stdout, "\t-a: Output should be AIFF, regardless of the input format\n");
			fprintf(stdout, "\t-c: Output should be CS229, regardless of the input format\n");
			return 0;
			
		} else if (strcmp(argv[i], "-1") == 0){ /* if the arg is -1, act as part 1*/
			basicSndconv();
			
		} else if ((strcmp(argv[i], "-a") == 0)){ /* force output to be AIFF */
			forceAIFF = 1;
			forceCS229 = 0;
			
		} else if ((strcmp(argv[i], "-c") == 0)){ /* force output to be AIFF */
			forceCS229 = 1;
			forceAIFF = 0;
			
		} else {  /*invalid switch */
			fprintf(stderr, "Error: invalid argument");
			exit(EXIT_FAILURE);
		}
	}/* treat standard input as file */
	soundfile_t from;
	initSoundfile(&from);
	
	strcpy(from.name, "(standard input)"); /* read from standard input */
	from.stream = stdin;
	
	setFormat(&from);
	if(strcmp(from.format, "CS229") == 0){
		parseCs229(&from);
	} else {
		parseAiff(&from);
	}
	fclose(from.stream);
	
	if(!forceCS229 && ((strcmp(from.format, "CS229") == 0) || forceAIFF)){
		writeAiff(&from, stdout);
	} else {
		writeCs229(&from, stdout);
	}
	
	return 0;
}