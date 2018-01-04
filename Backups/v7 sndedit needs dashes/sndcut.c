/* Author: Seth George */

#include "aiffutil.h"
#include "cs229util.h"

/**
*	reads a sound file from the standard input stream,
*	and writes the sound file to the standard output stream in the same format,
*	after removing all samples specified as arguments.
*/
int main(int argc, char *argv[]){
	soundfile_t snd;
	initSoundfile(&snd);
	int low[argc - 1];
	int high[argc - 1];	
	int i;
	int j;
	for(i = 1; i < argc; i++){ /* check for switches */
		if(strcmp(argv[i], "-h") == 0){ /* if the arg is -h, display help screen */
			fprintf(stderr, "\nsndcut reads a sound file from the standard input stream,\n");
			fprintf(stderr, "and writes the sound file to the standard output stream in the same format,\n");
			fprintf(stderr, "after removing all samples specified as arguments.\n");
			fprintf(stderr, "Any messages (e.g., errors) are written to standard error.\n");
			fprintf(stderr, "Specifically, arguments may be passed as:\n");
			fprintf(stderr, "\t$ sndcut [switches] [low..high] [low..high] ...\n");
			fprintf(stderr, "where low..high specifies that all samples numbered between low and high,\n");
			fprintf(stderr, "including the samples at low and high themselves, are to be removed.\n");
			fprintf(stderr, "This program supports the following switches, passed as arguments.\n");
			fprintf(stderr, "\t-h: Display a short help screen to standard error, and then terminate cleanly.\n");
			return 0;
			
		} else { /* take in the [low..high] args */
			for(j = 0; j < strlen(argv[i]); j++){ /* check that they are numbers or periods */
				if(!((argv[i][j] >= '0' && argv[i][j] <= '9') || argv[i][j] == '.')){
					fprintf(stderr, "Error: argument for range is not a number or is in incorrect format: %s\n", argv[i]);
					exit(EXIT_FAILURE);
				}
			}
			
			low[i -1] = atoi(strtok(argv[i], ".")); /* low equals the value before .. in [low..high] */
			char* dot = strtok(NULL," "); /* gets the second dot and int value */
			if(strncmp(dot, ".", 1) != 0){ /* check that it is a . */
				fprintf(stderr, "Error: invalid delimiter between extremes\n");
				exit(EXIT_FAILURE);
			}
			dot++; /* move past . and to only the int value */
			high[i -1] = atoi(dot); /* high equals the value after .. in [low..high] */
			
			if(low[i -1] > high[i -1]){
				fprintf(stderr, "Error: specified low value, %d,  is greater than high value, %d\n", low[i], high[i]);
				exit(EXIT_FAILURE);
			}
		}
	}
	
	snd.stream = stdin; /* read from standard input */
	setFormat(&snd); /* get info from file */
	if(strcmp(snd.format, "AIFF") == 0){
		parseAiff(&snd);
	} else {
		parseCs229(&snd);
	}
	
	int *data = malloc(snd.samples * snd.channels * sizeof(int));
	int count = 0;
	int k;
	for(i = 0; i < snd.samples; i++) { /* for each sample */
		for(j = 0; j < argc - 1; j++) { /* go through the [low..high] values */
			if((i >= low[j]) && (i <= high[j])){ /* if it is in "cut" range */
				i = high[j] + 1;
			}
		}
		if(i >= snd.samples) {
			break;
		}
		for(k = 0;  k < snd.channels; k++){ /*add every channel in the sample */
			data[count] = snd.sampleData[i*snd.channels + k];
			count++;
		}
		
	}
	
	data = realloc(data, count * sizeof(int)); /* cut off the extra off the end */
	snd.sampleData = data; /* point to the modified data */
	snd.samples = count/snd.channels;
	snd.numBytes = snd.channels*snd.samples*snd.bitDepth/8;
	
	fclose(snd.stream);
	
	if(strcmp(snd.format, "AIFF") == 0){
		writeAiff(&snd, stdout);
	} else {
		writeCs229(&snd, stdout);
	}
	
	return 0;
}