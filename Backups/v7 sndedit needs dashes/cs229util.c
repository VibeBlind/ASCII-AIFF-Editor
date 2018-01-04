/* Author: Seth George */

#include "cs229util.h"

/**
*	Parses CS229 files 
*/
void parseCs229(soundfile_t *snd){
	parseHeader(snd);
	parseData(snd);
}

/**
*	Parses the header of CS229 files, checking for keywords and their corresponding values.
*	Stops once StartData is found.
*/
void parseHeader(soundfile_t *snd){
	char keyword[16]; /* token held for comparison */
	while(strcmp(keyword, "StartData") != 0){
		fscanf(snd->stream, "%s", keyword);
		if(feof(snd->stream)){ /* check if end of file */
			fprintf(stderr, "Error: found end of file before StartData\n");
			exit(EXIT_FAILURE);
		}
		if(strcmp(keyword, "SampleRate") == 0){	
			if(snd->sampleRate){
				fprintf(stderr, "Error: duplicate keyword SampleRate\n");
				exit(EXIT_FAILURE);
			}
			
			snd->sampleRate = parseKeywordValue(snd->stream, 0);
			parseNewLine(0, snd->stream);
		} else if(strcmp(keyword, "BitDepth") == 0){			
			if(snd->bitDepth){
				fprintf(stderr, "Error: duplicate keyword BitDepth\n");
				exit(EXIT_FAILURE);
			}
			snd->bitDepth = parseKeywordValue(snd->stream, 0);
			parseNewLine(0, snd->stream);	
		} else if(strcmp(keyword, "Channels") == 0){			
			if(snd->channels){
				fprintf(stderr, "Error: duplicate keyword Channels\n");
				exit(EXIT_FAILURE);
			}
			snd->channels = parseKeywordValue(snd->stream, 0);
			parseNewLine(0, snd->stream);
		} else if(strcmp(keyword, "Samples") == 0){			
			if(snd->samples != -1){
				fprintf(stderr, "Error: duplicate keyword Samples\n");
				exit(EXIT_FAILURE);
			}
			snd->samples = parseKeywordValue(snd->stream, 1);
			parseNewLine(0, snd->stream);
		} else if(strcmp(keyword, "#") == 0){
			parseNewLine(1, snd->stream); /* skip everything after # */
		} else if(strcmp(keyword, "StartData") == 0){ /* will leave the loop next iteration */
			keywordsAreSet(snd); /* check if necessary keywords are set to not 0 */
		} else {
			fprintf(stderr, "Error: invalid keyword found: %s\n", keyword);
			exit(EXIT_FAILURE);
		}
	}
}

/**
*	Parses for a positive int value
*/
int parseKeywordValue(FILE *stream, int isSamples){
	int value = 0;
	int isNum = 0;
	
	isNum = fscanf(stream, "%d", &value);
	if(isNum != 0){ /* check that fscanf returned true */
		if(isNum == EOF){ /* check fscanf did not return end of file */
			fprintf(stderr, "Error: found end of file before StartData\n");
			exit(EXIT_FAILURE);
		}
		
		if(value <= 0 && !isSamples){ /*if keyword is negative or not Samples */
			fprintf(stderr, "Error: keyword value must be non-zero positive");
			exit(EXIT_FAILURE);
		} else if(value < 0){ /*Samples can be zero */
			fprintf(stderr, "Error: keyword value must be positive");
			exit(EXIT_FAILURE);
		}
	}
	return value;
}

/**
*	Finds the end of a line
*/
void parseNewLine(int isComment, FILE *stream){
	char c = 0;
	while(c != '\n'){
		c = fgetc(stream);
		if(c == EOF){ /* check if is end of file */
			fprintf(stderr, "Error: found end of file before StartData\n");
			exit(EXIT_FAILURE);
		}
		if(!isComment){
			if(!isspace(c)){ /* check if is not white space*/
				fprintf(stderr, "Error: unexpected character after value");
				exit(EXIT_FAILURE);
			}
		}
	}
}

/**
*	Parses through all the samples, checking for errors
*/
void parseData(soundfile_t *snd){
	int value; /* holds sample value */
	int found = 0;
	int dataPoints = 0;
	
	if(snd->samples > 0){ /* if samples are specified and not zero */
		dataPoints = snd->samples * snd->channels;
		snd->sampleData = malloc(dataPoints * sizeof(int));
		int num;
		while((num = fscanf(snd->stream, "%d", &value)) != EOF){ /*while it is not end of file */
			if(num < 1){ /* check if an int could be found */
				fprintf(stderr, "Error: invalid value found %c\n", num);
				exit(EXIT_FAILURE);
			}
			
			if(found >= dataPoints){ /* if there are too many */
				fprintf(stderr, "Error: specified number of samples, %d, was more than read number of samples, %d\n", found, dataPoints);
				exit(EXIT_FAILURE);
			}
			
			checkBitDepth(num, snd->bitDepth);
			snd->sampleData[found] = value;
			found++; /* found used as index */
		}
		
		if(found < dataPoints){ /* if there are not enough */
			fprintf(stderr, "Error: specified number of samples, %d, was fewer than read number of samples, %d\n", found, dataPoints);
			exit(EXIT_FAILURE);
		}
	} else {
		int num;
		snd->sampleData = malloc(0);		
		while((num = fscanf(snd->stream, "%d", &value)) != EOF){ /*while it is not end of file */
			if(num < 1){ /* check if an int could be found */
				fprintf(stderr, "Error: invalid value found %c\n", num);
				exit(EXIT_FAILURE);
			}
			dataPoints += snd->channels;
			snd->sampleData = realloc(snd->sampleData, (dataPoints * sizeof(int)));
			if(snd->sampleData == NULL) {
				fprintf(stderr, "Error: error allocating memory for sample data.\n");
				exit(EXIT_FAILURE);
			}
			
			checkBitDepth(num, snd->bitDepth);
			snd->sampleData[found] = value;
			found++; /* found used as index */
			
			int i;
			for(i = 1; i < snd->channels; i++){
				if((num = fscanf(snd->stream, "%d", &value)) == EOF){
					fprintf(stderr, "Error: found end of file before reading all channels\n");
					exit(EXIT_FAILURE);
				}
				
				if(num < 1){ /* check if an int could be found */
					fprintf(stderr, "Error: invalid value found %c\n", num);
					exit(EXIT_FAILURE);
				}
				
				checkBitDepth(num, snd->bitDepth);
				snd->sampleData[found] = value;
				found++; /* found used as index */
			}
		}
		snd->samples = found/snd->channels;
	}
}

/**
*	Write an CS229 file from a AIFF file
*/
void writeCs229(soundfile_t *from, FILE* to){
	fprintf(to, "CS229\n"); /* print the gathered values from the parsed file */
	fprintf(to, "\nSampleRate %d\n", from->sampleRate);
	fprintf(to, "Samples %d\n", from->samples);
	fprintf(to, "BitDepth %d\n", from->bitDepth);
	fprintf(to, "Channels %d\n", from->channels);
	fprintf(to, "\nStartData\n");
	
	int i;
	int j;
	int value;
	int bytes = from->bitDepth/8;
	
	for(i = 0; i < from->samples; i++){ /* print all the samples */
		for(j = 0; j < from->channels; j++){
			value = from->sampleData[i*from->channels +j]; /* get sample from correct channel */
			fprintf(to, "%d\t", value);
		}
		fprintf(to, "\n");
	}
}