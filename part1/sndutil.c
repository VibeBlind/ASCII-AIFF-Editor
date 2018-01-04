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
*	Duration is broken up into hrs:mins:secs:fracs 
*/
void printSndInfo(soundfile_t *snd){
	float secs = snd->duration;
	int hrs;
	int mins;

	hrs = secs / 3600;
	secs -= hrs * 3600;
	mins = secs / 60;
	secs -= mins * 60;
	
	printf("----------------------------------------------------------------------\n");
	printf("Filename: %s\nFormat: %s\nSampleRate: %d\nBitDepth: %d\nChannels: %d\nSamples: %d\nDuration: %d:%d:%.2f\n", snd->name, snd->format, snd->sampleRate, snd->bitDepth, snd->channels, snd->samples, hrs, mins, secs);
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
		char test[6];
		test[6] = 0;
		
		fgets(test, 5, snd->fp);
		/* compare the first 4 bytes */
		if(!strncmp("FORM", test, 4)){ 
			strncpy(snd->format, "AIFF", 6);
			snd->format[5] = 0;
		} else if(!strncmp("CS22", test, 4)){
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
	int value;	
	int numSamples = 0;
	int isNum;
	
	while((isNum = fscanf(snd->fp, "%d", &value)) != EOF){
		if(isNum == 0){ /* check that fscanf returned false */
			fprintf(stderr, "Error: invalid sample value\n");
			exit(1);
		}
		
		if((value < (-1*(power(2, snd->bitDepth -1))))||(value > (power(2, snd->bitDepth -1) -1))){
			fprintf(stderr, "Error: found sample beyond excepted bit depth limits\n");
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
	}
	
	snd->samples = numSamples;
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
				fprintf(stderr, "Error: found sample beyond excepted bit depth limits\n");
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
	int remaining = bytesToInt(snd, 4); /* get the size of the file */
	char buf[4];
	if(fread(buf, 1,  4, snd->fp) == EOF){
		fprintf(stderr, "Error: found end of file before was specified\n");
	}
	
	int test = strncmp(buf, "AIFF", 4);
	printf("%c %c %c %c %d", buf[0], buf[1], buf[2], buf[3], test);
	
	if(!strncmp(buf, "AIFF", 4){ /* check that the next 4 bytes are "AIFF" */
		fprintf(stderr, "Error: AIFF not found, invalid format\n");
		exit(1);
	}
	remaining -= 4;
	
	do{ /* while there are bytes remaining */
		if(fread(buf, 1,  4, snd->fp) == EOF){ /* read 4 bytes for ID */
			fprintf(stderr, "Error: found end of file before was specified\n");
			exit(1);
		}
		remaining -= 4;
		
		if(strncmp(buf, "COMM", 4)){ /* check that the next 4 bytes are "COMM" or "SSND" */			
			remaining -= parseComm(snd); /* read stuff and then skip to end of chunk */
		} else if(strncmp(buf, "SSND", 4)){
			remaining -= parseSsnd(snd); /* read stuff and then skip to end of chunk */
		} else {
			int chunkSize = bytesToInt(snd, 4);
			if(chunkSize % 2 == 1){ /* add 1 byte to this number if it is odd */
				chunkSize++;
			}
			fseek(snd->fp, chunkSize, SEEK_CUR); /* if it is not, skip the whole chunk */
			remaining -= chunkSize;
		}
	} while(remaining > 0);
}

/**
*	Parse the Common chunk of AIFF files
*/
int parseComm(soundfile_t *snd){
	int chunkSize = bytesToInt(snd, 4);
	if(chunkSize % 2 == 1){ /* add 1 byte to this number if it is odd */
		chunkSize++;
	}
	int leftInChunk = chunkSize;
	
	snd->channels = bytesToInt(snd, 2); /* NumChannels (2 bytes) */
	leftInChunk -= 2;
	snd->samples = bytesToInt(snd, 4); /* NumSampleFrames (4 bytes, unsigned) */
	leftInChunk -= 4;
	snd->bitDepth = bytesToInt(snd, 2); /* SampleSize (2 bytes) */
	leftInChunk -= 2;
	snd->sampleRate = floatToLong(snd); /* SampleRate (10 bytes) */
	leftInChunk -= 10;
	
	keywordsAreSet(snd);
	fseek(snd->fp, leftInChunk, SEEK_CUR);
	return chunkSize + 22;
}

/**
*	Parse the Sound chunk of AIFF files
*/
int parseSsnd(soundfile_t *snd){
	int chunkSize = bytesToInt(snd, 4);
	if(chunkSize % 2 == 1){ /* add 1 byte to this number if it is odd */
		chunkSize++;
	}
	snd->offset = bytesToInt(snd, 4); /* Offset (4 bytes, unsigned) */
	snd->blockSize = bytesToInt(snd, 4); /* Block Size (4 bytes, unsigned) */
	
	int value;
	int i;
	int upper = power(2, snd->bitDepth -1) -1;
	int lower = -1*(power(2, snd->bitDepth -1));
	for(i = chunkSize; i > 0; i--){ /* check if within bitdepth bounds */
		value = bytesToInt(snd, snd->bitDepth/8);
		if((value < lower)||(value > upper)){
			fprintf(stderr, "Error: found sample beyond excepted bit depth limits\n");
			exit(1);
		}
	}
	return chunkSize;
}

/*==================== Byte Stuff ====================*/
/**
*	Read number of bytes as an int
*/
int bytesToInt(soundfile_t *snd, int numBytes){
	unsigned char* buffer = malloc(numBytes);
	if(fread(buffer, numBytes, 1, snd->fp) == EOF){
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
unsigned long floatToLong(soundfile_t *snd){
	unsigned char* buffer = malloc(10);
	if(fread(buffer, 1,  10, snd->fp) == EOF){
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
	char flip[numBytes];
	int i;
	for(i = 0; i < numBytes; i++){
		flip[i] =  buffer[numBytes -i -1];
	}
	return *((int*)flip);
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
*/
writeAiff(soundfile_t *from, FILE *to){
	fprintf(to, "FORM");
	
}

/**
*	Write an CS229 file from a AIFF file
*/
writeCs229(soundfile_t *from, FILE* to){
	fprintf(to, "CS229");
	fprintf(to, "SampleRate %d", from->sampleRate);
	fprintf(to, "Samples %d", from->samples);
	fprintf(to, "BitDepth %d", from->bitDepth);
	fprintf(to, "Channels %d", from->channels);
	fprintf(to, "StartData");
	
	printf("CS229");
	printf("SampleRate %d", from->sampleRate);
	printf("Samples %d", from->samples);
	printf("BitDepth %d", from->bitDepth);
	printf("Channels %d", from->channels);
	printf("StartData");
	
	/*int i;
	int j;
	int num;
	for(i = 0; i > from->samples; i++){
		for(j = 0; j > from->channels; j++){
			num = byteToInt(from, from->bitDepth/8);
			fprintf(to, "%d\t", num);
		}
		fprintf(to, "\n");
	}*/
}