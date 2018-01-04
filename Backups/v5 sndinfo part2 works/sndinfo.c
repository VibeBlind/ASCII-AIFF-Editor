/* Author: Seth George */

#include "aiffutil.h"
#include "cs229util.h"

int main(int argc, char *argv[]){
	int i;
	for(i = 1; i < argc; i++){
		if(strcmp(argv[i], "-h") == 0){ /* if the arg is -h, display help screen*/
			fprintf(stdout, "\nsndinfo reads a given file or files and prints a list of stats about the file to stdout.\n");
			fprintf(stdout, "This program supports passing the file names and other switches as arguments.\n");
			fprintf(stdout, "Arguments may be passed as:\n");
			fprintf(stdout, "\t$ sndinfo [switches] [file] [file] ...\n");
			fprintf(stdout, "where brackets indicate optional arguments. For each specified input file,\n");
			fprintf(stdout, "If no file names are specified, then sndinfo treats the standard input stream\n");
			fprintf(stdout, "as the file, and show a file name of \(standard input).\n");
			fprintf(stdout, "\t This program supports the following switches, passed as arguments.\n");
			fprintf(stdout, "\t-h: Displays a short help screen to standard error, and terminates cleanly\n");
			fprintf(stdout, "\t-1: Prompt for a file name rather than accepting it as an argument\n");
			return 0;
			
		} else if(strcmp(argv[i], "-1") == 0){ /* if the arg is -1, act as part 1*/
			basicSndinfo();
			
		} else { /* you are reading a file name as an argument */
			soundfile_t snd;
			initSoundfile(&snd);
			
			snd.stream = fopen(argv[i], "r"); /* argv[1] should be a file name to open */
			strcpy(snd.name, argv[i]); /* set name to argument */
			
			if(!snd.stream){ /* invalid file pointer */
				fprintf(stderr, "Error: no such file\n");
				exit(EXIT_FAILURE);
			}

			setFormat(&snd);
			if(strcmp(snd.format, "CS229") == 0){
				parseCs229(&snd);
			} else {
				parseAiff(&snd);
			}
			fclose(snd.stream); /* close file after parsing */
			
			snd.duration = ((float)snd.samples)/snd.sampleRate; /* duration in seconds */
			printSndInfo(&snd);
		}
	}
	
	if(argc == 1){ /* if no switches or files were given */
		soundfile_t snd;
		initSoundfile(&snd);
		
		strcpy(snd.name, "(standard input)"); /* read from standard input */
		snd.stream = stdin;
		
		setFormat(&snd);
		if(strcmp(snd.format, "CS229") == 0){
			parseCs229(&snd);
		} else {
			parseAiff(&snd);
		}
		
		fclose(snd.stream); /* close file after parsing */
		snd.duration = ((float)snd.samples)/snd.sampleRate; /* duration in seconds */
		
		printSndInfo(&snd);
	}
	
	printf("----------------------------------------------------------------------\n");	
	return 0;
}