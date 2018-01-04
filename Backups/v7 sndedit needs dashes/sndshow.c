/* Author: Seth George */

#include "aiffutil.h"
#include "cs229util.h"

/**
*	reads a sound file from the standard input stream,
*	and displays an ASCII art representation of the sample data
*/
int main(int argc, char *argv[]){
	int c = -1; /* number of channels, -1 means all */
	int w = 80; /* total output width */
	int n = 1; /* zoom factor */
	
	int i;
	int j;
	for(i = 1; i < argc; i++){ /* check for switches */
		if(strcmp(argv[i], "-h") == 0){ /* if the arg is -h, display help screen */
			fprintf(stderr, "\nsndshow reads a sound file from the standard input stream,\n");
			fprintf(stderr, "and displays an ASCII art representation of the sample data formatted as follows:\n");
			fprintf(stderr, "The first 9 characters are used to display the sample number which are right justified.\n");
			fprintf(stderr, "Next comes a pipe character, |\n");
			fprintf(stderr, "The next x characters are used for plotting negative values.\n");
			fprintf(stderr, "Next comes a pipe character that represents the value 0.\n");
			fprintf(stderr, "The next x characters are used for plotting positive values.\n");
			fprintf(stderr, "The final character is another pipe character.\n");
			fprintf(stderr, "The value of x is dictated by the desired total output width; see the program switches.\n");
			fprintf(stderr, "For each sample frame, there will be c lines of output, where c is the number of channels.\n");
			fprintf(stderr, "The sample number is displayed only for the first line of output for the sample.\n");
			fprintf(stderr, "Then, for each channel, a line is drawn using the\n");
			fprintf(stderr, "dash character to represent the sample value for that channel.\n");
			fprintf(stderr, "A line segment is drawn between 0 and the sample value, then is overlaid x equal-width boxes between\n");
			fprintf(stderr, "0 and the maximum sample value, if the sample value is positive,\n");
			fprintf(stderr, "or between 0 and the minimum sample value, if the sample value is negative.\n");
			fprintf(stderr, "For each box that is half full or more, write a - character, otherwise write a space.\n");
			fprintf(stderr, "This program supports the following switches, passed as arguments.\n");
			fprintf(stderr, "\t-h: Display a short help screen to standard error, and then terminates cleanly.\n");
			fprintf(stderr, "\t-c c: Show the output only for channel c, for 1 =< c =< #channels.\n");
			fprintf(stderr, "\t-w w: Specify the total output width, in number of characters.\n");
			fprintf(stderr, "\t\t If not specified, the default is w = 80. Supports values down to w = 20\n");
			fprintf(stderr, "\t\t Note that w must be even so that the number of characters for representing positive values\n");
			fprintf(stderr, "\t\t is equal to the number of characters for representing negative values\n");
			fprintf(stderr, "\t-z n: Zoom out by a factor of n. If not specified, the default is n = 1\n");
			fprintf(stderr, "\t\t The value to plot should be the largest magnitude value over n consecutive samples,\n");
			fprintf(stderr, "\t\t and the number of lines of output should decrease by about a factor of n\n");
			return 0;
		
		} else if(strcmp(argv[i], "-c") == 0){ /* show the output only for channel c */
			i++;
			if(i == argc){
				fprintf(stderr, "Error: no value given after switch -c");
				exit(EXIT_FAILURE);
			}
			
			for(j = 0; j < strlen(argv[i]); j++){
				if(argv[i][j] <= '0' || argv[i][j] > '9'){ /* if not a number or zero*/
					fprintf(stderr, "Error: given c value, %s, is invalid\n", argv[i]);
					exit(EXIT_FAILURE);
				}
			}
			c = atoi(argv[i]); /* c equals the given value */

		} else if(strcmp(argv[i], "-w") == 0){ /* specify the total output width, in number of characters */
			i++;
			if(i == argc){
				fprintf(stderr, "Error: no value given after switch -w");
				exit(EXIT_FAILURE);
			}
			
			for(j = 0; j < strlen(argv[i]); j++){
				if(argv[i][j] < '0' || argv[i][j] > '9'){ /* if not a number or zero*/
					fprintf(stderr, "Error: given w value, %s, is invalid\n", argv[i]);
					exit(EXIT_FAILURE);
				}
			}
			int check = atoi(argv[i]); /* check equals the given value */
						
			if(check % 2){ /* if w is odd, round down */
				check--;
			}
			
			if(check < 20){ /* does not support values less than 20 */
				fprintf(stderr, "Error: does not support w values under 20\n");
				exit(EXIT_FAILURE);
			}
			
			w = check;
			
		} else if(strcmp(argv[i], "-z") == 0){ /* Zoom out by a factor of n, up to 20 */
			i++;
			if(i == argc){
				fprintf(stderr, "Error: no value given after switch -z");
				exit(EXIT_FAILURE);
			}
			
			for(j = 0; j < strlen(argv[i]); j++){
				if(argv[i][j] <= '0' || argv[i][j] > '9'){ /* if not a number or zero*/
					fprintf(stderr, "Error: given n value, %s, is invalid\n", argv[i]);
					exit(EXIT_FAILURE);
				}
			}
			n = atoi(argv[i]); /* n equal the given value */

		} else { /* invalid argument */
			fprintf(stderr, "Error: invalid argument %s", argv[i]);
			exit(EXIT_FAILURE);
		}
	}
	soundfile_t snd;
	initSoundfile(&snd);
	snd.stream = stdin; /* read from standard input */
	setFormat(&snd); /* get info from file */
	if(strcmp(snd.format, "AIFF") == 0){
		parseAiff(&snd);
	} else {
		parseCs229(&snd);
	}
	int displayed = 0; /* amount of channels displayed */
	if(c == -1){ /* if all the channels */
		displayed = (snd.samples * snd.channels)/n; /* display all the channels of the samples in regard to zoom factor */
		if((snd.samples * snd.channels) % n){
			displayed++;
		}
	} else if(c <= snd.channels){ /* if given channel is less or equal to the number of samples */
		displayed = snd.samples/n; /* display the selected channels in regard to zoom factor */
		if((snd.samples) % n){
			displayed++;
		}
	} else {
		fprintf(stderr, "Error: specified number of channels is greater than the number of samples\n");
		exit(EXIT_FAILURE);
	}
	
	int allDisplayed[displayed];
	int index = 0;
	for(i = 0; i < snd.samples; i += n){ /* go through the samples, increasing index by the zoom factor */
		for(j = 1; j <= snd.channels; j++){
			if((c == j) || (c == -1)){ /* display only the chosen channel or all the channels if -1*/
				allDisplayed[index] = zoomSample(i*snd.channels + (j - 1), n, &snd); /* the greatest magnitude sample value of n number of samples */
				index++;
			}
		}
	}
	
	/* print out the values */
	char *buffer = calloc(9, 1); /* first part is 9 chars long */
	for(i = 0; i < displayed/snd.channels; i++){ /* put the sample number in the first 9 chars */
		sprintf(buffer, "%d", i);
		for(j = 0; j < (9 - strlen(buffer)); j ++) { /* the first chars are spaces, then the number*/
			fprintf(stdout, " ");
		}
		fprintf(stdout, "%s|", buffer); /* the number should be "right justified" */
		printDisplayed(allDisplayed[i*snd.channels], snd.bitDepth, w); /* print out the dash bar representation */
		
		if(c == -1) { /* for more than 1 channel */
			for(j = 1; j < snd.channels; j++){
				fprintf(stdout, "         |"); /* for multiple channels, we don't show a number */
				printDisplayed(allDisplayed[i*snd.channels + j] , snd.bitDepth, w); /* print out the dash bar representation */
			}	
		}
	}
	
	return 0;
}