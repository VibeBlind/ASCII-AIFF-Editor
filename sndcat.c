/* Author: Seth George */

#include "aiffutil.h"
#include "cs229util.h"

/**
*	Reads all sound files passed as arguments,
*	and writes a single sound file where the sample data
*	is the concatenation of the sample data in the inputs
*/
int main(int argc, char *argv[]){
	soundfile_t combined;
	int isFirst = 1;
	
	int forceAIFF = 0; /* flag for force aiff */
	int forceCS229 = 0; /* flag for force cs229 */	
	int i;
	for(i = 1; i < argc; i++){ /* check for switches */
		if(strcmp(argv[i], "-h") == 0){ /* if the arg is -h, display help screen */
			fprintf(stderr, "\nsndcat reads all sound files passed as arguments, and writes a single sound file\n");
			fprintf(stderr, "where the sample data is the concatenation of the sample data in the inputs.\n");
			fprintf(stderr, "The resulting sound file are written directly to standard output.\n");
			fprintf(stderr, "If no files are passed as arguments, then the standard input stream are treated as the input file.\n");
			fprintf(stderr, "Any messages (e.g., errors) are written to standard error.\n");
			fprintf(stderr, "This program supports the following switches, passed as arguments.\n");
			fprintf(stderr, "\t-h: Displays a short help screen to standard error, and then terminate cleanly.\n");
			fprintf(stderr, "\t-a: Force output to be AIFF\n");
			fprintf(stderr, "\t-c: Force output to be CS229\n");
			return 0;
			
		} else if ((strcmp(argv[i], "-a") == 0)){ /* force output to be AIFF */
			forceAIFF = 1;
			forceCS229 = 0;			
		} else if ((strcmp(argv[i], "-c") == 0)){ /* force output to be AIFF */
			forceCS229 = 1;
			forceAIFF = 0;			
		} else { /* take in a file name as an argument */	
			soundfile_t snd;
			initSoundfile(&snd);
			
			snd.stream = fopen(argv[i], "r"); /* open file as read only */
			if(!snd.stream){ /* invalid file pointer */
				fprintf(stderr, "Error: invalid file argument, cannot be opened.\n");
				exit(EXIT_FAILURE);
	
			}
			setFormat(&snd); /* should throw error if not CS229 or AIFF */
			
			if(strcmp(snd.format, "CS229") == 0){
				parseCs229(&snd);
			} else {
				parseAiff(&snd);
			}
			fclose(snd.stream);
			
			if(isFirst){ /* if is the first sndfile given */
				combined = snd;
				isFirst = 0;
			} else {
				if((combined.sampleRate != snd.sampleRate) || (combined.bitDepth != snd.bitDepth) || (combined.channels != snd.channels)){
					fprintf(stderr, "Error: input argument sample rate, bit depth, or channels did not match of first file");//TODO
					exit(EXIT_FAILURE);
				}
				
				/* add samples of this file to be combined */
				int oldSize = combined.samples * combined.channels; 
				combined.samples += snd.samples;
				int newSize = combined.samples * combined.channels;
				combined.sampleData = realloc(combined.sampleData, newSize * sizeof(int));
			
				if(!combined.sampleData) { /* if statement to check that memory could be reallocated */
					fprintf(stderr, "Error: was unable to allocate memory");
					exit(EXIT_FAILURE);
				}
				int j;
				for(j = oldSize; j < newSize; j++) {
					combined.sampleData[j] = snd.sampleData[j - oldSize];
				}
			}
		}
	}
	
	if(argc == 1){ /* if no files were specified */
		initSoundfile(&combined);
		combined.stream = stdin; /* read from standard input */
		
		setFormat(&combined);
		if(strcmp(combined.format, "CS229") == 0){
			parseCs229(&combined);
		} else {
			parseAiff(&combined);
		}
		fclose(combined.stream);
	}
	
	/* write the new sndcat'd file */
	if(!forceCS229 && ((strcmp(combined.format, "AIFF") == 0) || forceAIFF)){
		writeAiff(&combined, stdout);
	} else {
		writeCs229(&combined, stdout);
	}
	
	return 0;
}