/* Author: Seth George */

#include "sndlib.h"

/*======================================================================**
**																		**
**						General Code Functions							**	
**																		**
**======================================================================*/
/**
*	Set all int variables in struct to zero
*/
void initSoundfile(soundfile_t *snd){
	snd->samples = 0;
	snd->sampleRate = 0;
	snd->bitDepth = 0;
	snd->channels = 0;
	snd->duration = 0;
}

/**
*	Check if sample rate, bit depth, and channels are set
*/
void keywordsAreSet(soundfile_t *snd){
	if((snd->sampleRate == 0)||(snd->bitDepth == 0)||(snd->channels == 0)){
		fprintf(stderr, "Error: Missing required keyword\n");
		exit(1);
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
	printf("----------------------------------------------------------------------\n");
}

/**
*	Determines file format
*/
void setFormat(soundfile_t *snd){
	if(!snd->fp){ /* invalid file pointer */
		fprintf(stderr, "Error: no such file\n");
		exit(1);
	} else { /* valid file pointer */
		char test[5];
		fgets(test, 5, snd->fp);
		/* compare the first 4 bytes */
		if(strncmp("FORM", test, 4) == 0){ 
			strncpy(snd->format, "AIFF", 6);
			snd->format[5] = 0;
		} else if(strncmp("CS22", test, 4) == 0){
			if(fgetc(snd->fp) == '9'){ /* check if the next byte is 9 */
				strncpy(snd->format, "CS229", 6);
			} else {
				fprintf(stdout, "Error: Invalid File Format\n");
				exit(1);
			}
		} else {
			fprintf(stdout, "Error: Invalid File Format\n");
			exit(1);
		}
	}
}


/*======================================================================**
**																		**
**						Parsing C229 Functions							**	
**																		**
**======================================================================*/
/**
*	Parses CS229 files 
*/
void parseCs229(soundfile_t *snd){
	parseHeader(snd);
	parseData(snd);
}

/*==================== Keywords stuff ====================*/
/**
*	Parses the header of CS229 files, checking for keywords and their corresponding values.
*	Stops once StartData is found.
*/
void parseHeader(soundfile_t *snd){
	char keyword[16]; /* token held for comparison */
	while(strcmp(keyword, "StartData") != 0){
		if(fscanf(snd->fp, "%s", keyword)== EOF){ /* check if end of file */
			fprintf(stderr, "Error: found end of file before StartData\n");
			exit(1);
		}
		if(!strcmp(keyword, "SampleRate")){	
			if(snd->sampleRate){
				fprintf(stderr, "Error: duplicate keyword specifier\n");
				exit(1);
			}
			snd->sampleRate = parseKeywordValue(snd->fp);
			parseNewLine(0, snd->fp);

		} else if(!strcmp(keyword, "BitDepth")){			
			if(snd->bitDepth){
				fprintf(stderr, "Error: duplicate keyword specifier\n");
				exit(1);
			}
			snd->bitDepth = parseKeywordValue(snd->fp);
			parseNewLine(0, snd->fp);
			
		} else if(!strcmp(keyword, "Channels")){			
			if(snd->channels){
				fprintf(stderr, "Error: duplicate keyword specifier\n");
				exit(1);
			}
			snd->channels = parseKeywordValue(snd->fp);
			parseNewLine(0, snd->fp);
			
		} else if(!strcmp(keyword, "Samples")){			
			if(snd->samples){
				fprintf(stderr, "Error: duplicate keyword specifier\n");
				exit(1);
			}
			snd->samples = parseKeywordValue(snd->fp);
			parseNewLine(0, snd->fp); 
		} else if(!strcmp(keyword, "#")){
			parseNewLine(1, snd->fp); /* skip everything after # */
		} else if(!strcmp(keyword, "StartData")){ /* will leave the loop next iteration */
			keywordsAreSet(snd); /* check if necessary keywords are set to not 0 */
		} else {
			fprintf(stderr, "Error: invalid keyword found: %s\n", keyword);
			exit(1);
		}
	}
}

/**
*	Parses for a positive int value
*/
int parseKeywordValue(FILE *fp){
	int value = 0;
	int isNum = 0;
	
	isNum = fscanf(fp, "%d", &value);
	if(isNum != 0){ /* check that fscanf returned true */
		if(isNum == EOF){ /* check fscanf did not return end of file */
			fprintf(stderr, "Error: found end of file before StartData\n");
			exit(1);
		}
		
		if(value <= 0){
			fprintf(stderr, "Error: keyword value must be positive");
			exit(1);
		}
	}
	return value;
}

/**
*	Finds the end of a line
*/
void parseNewLine(int isComment, FILE *fp){
	char c = 0;
	while(c != '\n'){
		c = fgetc(fp);
		if(c == EOF){ /* check if is end of file */
			fprintf(stderr, "Error: found end of file before StartData\n");
			exit(1);
		}
		if(!isComment){
			if(!isspace(c)){ /* check if is not white space*/
				fprintf(stderr, "Error: unexpected character after value");
				exit(1);
			}
		}
	}
}

/*==================== Reading Data ====================*/
/**
*	Parses through all the samples, checking for errors
*/
void parseData(soundfile_t *snd){
	int upper = power(2, snd->bitDepth -1) -1;
	int lower = -1*(power(2, snd->bitDepth -1)); /* check if within bitdepth limits */
	int value;	
	int numSamples = 0;
	int isNum;
	
	while((isNum = fscanf(snd->fp, "%d", &value)) != EOF){
		if(isNum == 0){ /* check that fscanf returned false */
			fprintf(stderr, "Error: invalid sample value\n");
			exit(1);
		}
		
		if((value < lower)||(value > upper)){
			fprintf(stderr, "Error: found sample beyond excepted bit depth limits3\n");
			exit(1);
		}
		
		int i;
		for(i = 1; i < snd->channels; i++){
			parseSampleValue(snd, numSamples);
		}
		
		numSamples ++;
	}
	
	if(snd->samples){
		if(snd->samples != numSamples){
			fprintf(stderr, "Error: specified number of samples does not match read number of samples\n");
			exit(1);
		}
	} else {
		snd->samples = numSamples;
	}
}

/**
*	Parse a sample from the data
*/
int parseSampleValue(soundfile_t *snd, int numSamples){
	int value;
	int isNum;
	int upper = power(2, snd->bitDepth -1) -1;
	int lower = -1*(power(2, snd->bitDepth -1));
	if((isNum = fscanf(snd->fp, "%d", &value)) != EOF){ /* check fscanf did not return end of file */
		if(isNum == 1){ /* check that fscanf found a value */
			if((value < lower)||(value > upper)){/* if sample is out of bit depth limits */
				fprintf(stderr, "Error: found sample beyond excepted bit depth limits1\n");
				exit(1);
			}
		} else {
			fprintf(stderr, "Error: invalid sample value\n");
			exit(1);
		}
	} else {
		fprintf(stderr, "Error: found end of file before reading all sample data\n");
		exit(1);
	}
	return value;
}

/*======================================================================**
**																		**
**						Parsing AIFF Functions							**	
**																		**
**======================================================================*/
/**
*	Parse files of AIFF format
*/
void parseAiff(soundfile_t *snd){
	int commFlag = 0;
	int ssndFlag = 0;
	int remaining = bytesToInt(snd->fp, 4); /* get the size of the file */
	char buf[4];

	if(fread(buf, 1,  4, snd->fp) == EOF){
		fprintf(stderr, "Error: found end of file before was specified\n");
	}
	
	if(strncmp(buf, "AIFF", 4) > 0){ /* check that the next 4 bytes are "AIFF" */
		fprintf(stderr, "Error: AIFF not found, invalid format\n");
		exit(1);
	}
	remaining -= 4;
	
	char *storage = NULL;
	do{ /* while there are bytes remaining */
		fprintf(stderr, "In while, remaining: %d\n", remaining);
		if(fread(buf, 1,  4, snd->fp) == EOF){ /* read 4 bytes for ID */
			fprintf(stderr, "Error: found end of file before was specified\n");
			exit(1);
		}
		remaining -= 4;
		if(strncmp(buf, "COMM", 4) == 0){ /* check that the next 4 bytes are "COMM" */
			if(commFlag == 1){
				fprintf(stderr, "Error: duplicate COMM data block found\n");
				exit(1);
			}
			remaining -= parseComm(snd); /* read stuff and then skip to end of chunk */
			commFlag = 1;
		} else if(strncmp(buf, "SSND", 4) == 0){ /* check that the next 4 bytes are "SSND" */ 
			if(ssndFlag == 1){
				fprintf(stderr, "Error: duplicate SSND data block found\n");
				exit(1);
			}		
			remaining -= parseSsnd(snd, &storage); /* read stuff and then skip to end of chunk */
			ssndFlag = 1;
		} else {
			int chunkSize = bytesToInt(snd->fp, 4);
			if(chunkSize % 2 == 1){ /* add 1 byte to this number if it is odd */
				chunkSize++;
			}
				
			if(fseek(snd->fp, chunkSize, SEEK_CUR)== EOF){ /* if it is not, skip the whole chunk */
				fprintf(stderr, "Error: found end of file before was specified\n");
				exit(1);
			}
			
			remaining -= (chunkSize +4);
		}
	} while(remaining > 0);
	if(commFlag == 0 || ssndFlag == 0){ /* check is missing necessary chunk */
		fprintf(stderr, "Error: necessary data block not found: ");
		if(commFlag == 0) fprintf(stderr, "COMM ");
		if(ssndFlag == 0) fprintf(stderr, "SSND ");
		fprintf(stderr, "\n");
		exit(1);
	}
	checkSamples(snd, storage);
}

/**
*	Parse the COMM chunk of AIFF files
*/
int parseComm(soundfile_t *snd){
	int chunkSize = bytesToInt(snd->fp, 4);
	if(chunkSize % 2 == 1){ /* add 1 byte to this number if it is odd */
		chunkSize++;
	}
	int leftInChunk = chunkSize;
	
	snd->channels = bytesToInt(snd->fp, 2); /* NumChannels (2 bytes) */
	leftInChunk -= 2;
	snd->samples = bytesToInt(snd->fp, 4); /* NumSampleFrames (4 bytes, unsigned) */
	leftInChunk -= 4;
	snd->bitDepth = bytesToInt(snd->fp, 2); /* SampleSize (2 bytes) */
	leftInChunk -= 2;
	snd->sampleRate = floatToLong(snd->fp); /* SampleRate (10 bytes) */
	leftInChunk -= 10;
	
	fseek(snd->fp, leftInChunk, SEEK_CUR);
	return chunkSize +4; /* 4 bytes of determining size */
}

/**
*	Parse the SSND chunk of AIFF files
*/
int parseSsnd(soundfile_t *snd, char **storage){
	int chunkSize = bytesToInt(snd->fp, 4);
	if(chunkSize % 2 == 1){ /* add 1 byte to this number if it is odd */
		chunkSize++;
	}
	int sndData = chunkSize;
	int offset = bytesToInt(snd->fp, 4); /* Offset (4 bytes, unsigned) */
	int blockSize = bytesToInt(snd->fp, 4); /* Block Size (4 bytes, unsigned) */
	sndData -= (offset + blockSize + 8);
	if(fseek(snd->fp, offset, SEEK_CUR)== EOF){ /* skip offset */
		fprintf(stderr, "Error: found end of file before was specified\n");
	}
	*storage = malloc(sndData);
	snd->numBytes = sndData;
	if(fread(*storage, 1, sndData, snd->fp) == EOF){ /* store the sound data to be read after file is parsed */
		fprintf(stderr, "Error: found end of file before was specified\n");
	}
	if(fseek(snd->fp, blockSize, SEEK_CUR)== EOF){ /* skip block size */
		fprintf(stderr, "Error: found end of file before was specified\n");
	}
	return chunkSize +4; /* 4 bytes of determining size */
}

/**
*	Check that all the sound samples in SSND are within bit depth limit
*/
void checkSamples(soundfile_t *snd, char *storage){
	int expectedSize = (snd->bitDepth/8) * snd->samples * snd->channels; /* calculate what the size is supposed to be */
	fprintf(stderr, "bitDepth: %d\n samples: %d\n channels: %d\n", snd->bitDepth, snd->samples, snd->channels);

	if(expectedSize != snd->numBytes){ /* if statement checking its the right size */
		fprintf(stderr, "Error: number of samples %d does not match expected sample size of %d\n", snd->numBytes, expectedSize);
		exit(1);
	}
	
	int bytes = snd->bitDepth/8;
	snd->sampleData = malloc(bytes * snd->numBytes);
	char sample[bytes];
	char tmp8;
	short tmp16;
	int value;
	int i;
	int j;
	for(i = 0; i < snd->numBytes/bytes; i++){
		for(j = 0; j < bytes; j++){
			fprintf(stderr, "sample math: %d\n", (j + i*bytes)); //////////////////////////////////////////////////////////////////
			sample[j] = storage[j + i*bytes];
			fprintf(stderr, "sample value: %d\n", value); ///////////////////////////////////////////////////////////////////////////////
		}
		value = flipEndianness(sample, bytes);
		
		if(snd->bitDepth == 8){ /* make sure that the value stays negative */
			tmp8 = value;
			value = tmp8;
		} else if (snd->bitDepth == 16){
			tmp16 = value;
			value = tmp16;
		}
		
		checkBitDepth(value, snd->bitDepth);
		snd->sampleData[i] = value;
	}
}

/**
*	check that the given sample is within the right bit depth
*/
void checkBitDepth(int sample, int bitDepth){
	int upper = power(2, bitDepth -1) -1;
	int lower = -1*(power(2, bitDepth -1));

	if((sample < lower)||(sample > upper)){ /* check if within bitDepth bounds */
		fprintf(stderr, "Error: found sample beyond excepted bit depth limits %d\n", sample);
		exit(1);
	}
}

/*==================== Byte Stuff ====================*/
/**
*	Read number of bytes as an int
*/
int bytesToInt(FILE* fp, int numBytes){
	char* buffer = malloc(numBytes);
	if(fread(buffer, 1, numBytes, fp) == EOF){
		fprintf(stderr, "Error: found end of file before was specified\n");
		exit(1);
	}
	int num = flipEndianness(buffer, numBytes);
	free(buffer);
	return num;
}

/**
*	Read number of bytes as a long
*/
unsigned long floatToLong(FILE* fp){
	unsigned char* buffer = malloc(10);
	if(fread(buffer, 1,  10, fp) == EOF){
		fprintf(stderr, "Error: found end of file before was specified\n");
		exit(1);
	}
	unsigned long num = ConvertFloat(buffer);
	free(buffer);
	return num;
}

/**
*	Flip bytes to opposing endianness
*/
int flipEndianness(char* buffer, int numBytes){
	char* flip = calloc(4, 1);
	int i;
	for(i = 0; i < numBytes; i++){
		flip[i] =  buffer[numBytes -i -1];
	}
	int val = *((int*)flip);
	free(flip);
	return val;
}

#define INTEL_CPU
#ifdef INTEL_CPU
/***************************** FlipLong() ******************************
 * Converts a long in "Big Endian" format (ie, Motorola 68000) to Intel
 * reverse-byte format, or vice versa if originally in Big Endian.
 ********************************************************************* */
void FlipLong(unsigned char * ptr){
   register unsigned char val;

   /* Swap 1st and 4th bytes */
   val = *(ptr);
   *(ptr) = *(ptr+3);
   *(ptr+3) = val;

   /* Swap 2nd and 3rd bytes */
   ptr += 1;
   val = *(ptr);
   *(ptr) = *(ptr+1);
   *(ptr+1) = val;
}
#endif

/*************************** FetchLong() *******************************
 * Fools the compiler into fetching a long from a char array.
 ********************************************************************* */
unsigned long FetchLong(unsigned long * ptr){
   return(*ptr);
}

/*************************** ConvertFloat() *****************************
 * Converts an 80 bit IEEE Standard 754 floating point number to an unsigned
 * long.
 ********************************************************************** */
unsigned long ConvertFloat(unsigned char * buffer){
   unsigned long mantissa;
   unsigned long last = 0;
   unsigned char exp;

#ifdef INTEL_CPU
   FlipLong((unsigned long*)(buffer+2));
#endif

   mantissa = FetchLong((unsigned long *)(buffer+2));
   exp = 30 - *(buffer+1);
   while (exp--)
   {
	 last = mantissa;
	 mantissa >>= 1;
   }
   if (last & 0x00000001) mantissa++;
   return(mantissa);
}


/*======================================================================**
**																		**
**							Writing Functions							**	
**																		**
**======================================================================*/
/**
*	Write an Aiff file from a CS229 file
*
writeAiff(soundfile_t *from, FILE *to){
	fprintf(to, "FORM");
}*/

/**
*	Write an CS229 file from a AIFF file
*/
writeCs229(soundfile_t *from, FILE* to){
	fprintf(to, "CS229\n");
	fprintf(to, "\nSampleRate %d\n", from->sampleRate);
	fprintf(to, "Samples %d\n", from->samples);
	fprintf(to, "BitDepth %d\n", from->bitDepth);
	fprintf(to, "Channels %d\n", from->channels);
	fprintf(to, "\nStartData\n");
	
	int i;
	int j;
	int value;
	int byteDepth = from->bitDepth/8;
	
	for(i = 0; i < from->samples; i++){
		for(j = 0; j < from->channels; j++){
			//TODO
			fprintf(to, "%d\t", value);
			fprintf(stderr, "%d\t", value); ////////////////////////////////////////////////////////////////////////////////////
		}
		fprintf(to, "\n");
		fprintf(stderr, "\n"); ///////////////////////////////////////////////////////////////////////////////////////////////
	}
}