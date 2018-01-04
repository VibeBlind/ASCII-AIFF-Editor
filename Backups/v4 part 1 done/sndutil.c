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
	snd->samples = -1;
	snd->sampleRate = 0;
	snd->bitDepth = 0;
	snd->channels = 0;
	snd->duration = 0;
	snd->numBytes = 0;
}

/**
*	Check if sample rate, bit depth, and channels are set
*/
void keywordsAreSet(soundfile_t *snd){
	if((snd->sampleRate == 0)||(snd->bitDepth == 0)||(snd->channels == 0)){
		fprintf(stderr, "Error: Missing required keyword\n");
		exit(EXIT_FAILURE);
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
	if(!snd->stream){ /* invalid file pointer */
		fprintf(stderr, "Error: no such file\n");
		exit(EXIT_FAILURE);
	} else { /* valid file pointer */
		char test[5];
		fgets(test, 5, snd->stream);
		/* compare the first 4 bytes */
		if(strncmp("FORM", test, 4) == 0){ 
			strncpy(snd->format, "AIFF", 6);
			snd->format[5] = 0;
		} else if(strncmp("CS22", test, 4) == 0){
			if(fgetc(snd->stream) == '9'){ /* check if the next byte is 9 */
				strncpy(snd->format, "CS229", 6);
			} else {
				fprintf(stdout, "Error: Invalid File Format\n");
				exit(EXIT_FAILURE);
			}
		} else {
			fprintf(stdout, "Error: Invalid File Format\n");
			exit(EXIT_FAILURE);
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

/*==================== Reading Data ====================*/
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


/*======================================================================**
**																		**
**						Parsing AIFF Functions							**	
**																		**
**======================================================================*/
/**
*	Parse files of AIFF format
*/
void parseAiff(soundfile_t *snd){
	char buf[4];
	int remaining = bytesToInt(snd->stream, 4); /* get the size of the file */
	
	fread(buf, 1,  4, snd->stream); /*take in AIFF */
	if(feof(snd->stream)){ /* check if end of file */
		fprintf(stderr, "Error: found end of file before was specified\n");
		exit(EXIT_FAILURE);
	}
	if(strncmp(buf, "AIFF", 4) > 0){ /* check that the next 4 bytes are "AIFF" */
		fprintf(stderr, "Error: AIFF not found, invalid format\n");
		exit(EXIT_FAILURE);
	}
	remaining -= 4;
	
	char *storage = NULL; /* Set sound data to null before stuff is put into it, for error checking */
	int chunkSize;
	int dataSize;
	int commFlag = 0;
	int ssndFlag = 0;
	while(remaining > 0){ /* while there are bytes remaining */
		fread(buf, 1,  4, snd->stream); /* read 4 bytes for ID */
		if(feof(snd->stream)){ /* check if end of file */
			fprintf(stderr, "Error: found end of file before was specified\n");
			exit(EXIT_FAILURE);
		}
		remaining -= 4;
		
		chunkSize = bytesToInt(snd->stream, 4);
		if(chunkSize % 2 == 1){ /* add 1 byte to this number if it is odd */
			chunkSize++;
		}
		remaining -= 4;
		
		if(strncmp(buf, "COMM", 4) == 0){ /* check that the next 4 bytes are "COMM" */
			if(commFlag == 1){
				fprintf(stderr, "Error: duplicate COMM data block found\n");
				exit(EXIT_FAILURE);
			}
			commFlag = 1;
			parseComm(snd, chunkSize); /* read stuff and then skip to end of chunk */
			
		} else if(strncmp(buf, "SSND", 4) == 0){ /* check that the next 4 bytes are "SSND" */ 
			if(ssndFlag == 1){
				fprintf(stderr, "Error: duplicate SSND data block found\n");
				exit(EXIT_FAILURE);
			}
			ssndFlag = 1;
			parseSsnd(snd, chunkSize, &storage); /* read stuff and then skip to end of chunk */
			
		} else {
			fseek(snd->stream, chunkSize, SEEK_CUR);
			if(feof(snd->stream)){ /* if it is not, skip the whole chunk */
				fprintf(stderr, "Error: found end of file before was specified2\n");
				exit(EXIT_FAILURE);
			}
		}
		remaining -= chunkSize;
	}
	if(commFlag == 0 || ssndFlag == 0){ /* check is missing necessary chunk */
		fprintf(stderr, "Error: necessary data block not found: ");
		if(commFlag == 0) fprintf(stderr, "COMM ");
		if(ssndFlag == 0) fprintf(stderr, "SSND ");
		fprintf(stderr, "\n");
		exit(EXIT_FAILURE);
	}
	
	if(snd->samples){ /* samples my be 0 */
		checkSamples(snd, &storage);
	}
}

/**
*	Parse the COMM chunk of AIFF files
*/
void parseComm(soundfile_t *snd, int chunkSize){	
	snd->channels = bytesToInt(snd->stream, 2); /* NumChannels (2 bytes) */
	chunkSize -= 2;
	snd->samples = bytesToInt(snd->stream, 4); /* NumSampleFrames (4 bytes, unsigned) */
	chunkSize -= 4;
	snd->bitDepth = bytesToInt(snd->stream, 2); /* SampleSize (2 bytes) */
	chunkSize -= 2;
	snd->sampleRate = floatToLong(snd->stream); /* SampleRate (10 bytes) */
	chunkSize -= 10;
	
	fseek(snd->stream, chunkSize, SEEK_CUR); /* skip rest  block size */
	if(feof(snd->stream)){
		fprintf(stderr, "Error: found end of file before was specified\n");
	}
}

/**
*	Parse the SSND chunk of AIFF files
*/
void parseSsnd(soundfile_t *snd, int chunkSize, char **storage){
	int offset = bytesToInt(snd->stream, 4); /* Offset (4 bytes, unsigned) */
	int blockSize = bytesToInt(snd->stream, 4); /* Block Size (4 bytes, unsigned) */
	chunkSize -= (offset + blockSize + 8);
	
	fseek(snd->stream, offset, SEEK_CUR);/* skip offset */
	if(feof(snd->stream)){
		fprintf(stderr, "Error: found end of file before was specified\n");
		exit(EXIT_FAILURE);
	}

	*storage = malloc(chunkSize);
	snd->numBytes = chunkSize;
	fread(*storage, 1, chunkSize, snd->stream); /* store the sound data to be read after file is parsed */
	if(feof(snd->stream)){
		fprintf(stderr, "Error: found end of file before was specified\n");
		exit(EXIT_FAILURE);
	}
	
	fseek(snd->stream, blockSize, SEEK_CUR); /* skip block size to align block */
	if(feof(snd->stream)){
		fprintf(stderr, "Error: found end of file before was specified\n");
		exit(EXIT_FAILURE);
	}
}

/**
*	Check that all the sound samples in SSND are within bit depth limit
*/
void checkSamples(soundfile_t *snd, char **storage){
	int bytes = snd->bitDepth/8;	
	int expectedSize = bytes * snd->samples * snd->channels; /* calculate what the size is supposed to be */
	
	if(expectedSize != snd->numBytes){ /* if statement checking its the right size */
		fprintf(stderr, "Error: number of samples %d does not match expected sample size of %d\n", snd->numBytes, expectedSize);
		exit(EXIT_FAILURE);
	}
	
	snd->sampleData = malloc((snd->numBytes/bytes)*sizeof(int));
	char sample[bytes];
	int value;
	int i;
	int j;
	
	for(i = 0; i < snd->numBytes/bytes; i++){ /* for each data point */
		for(j = 0; j < bytes; j++){
			sample[j] = (*storage)[j + i*bytes];
		}
		value = flipEndianness(sample, bytes);
		
		if(value >> (snd->bitDepth -1)){
			value = ((value-1)^(power(2, snd->bitDepth)-1));
			value *= -1;
		}
		
		checkBitDepth(value, snd->bitDepth);
		snd->sampleData[i] = value;
	}
	free(*storage);
}

/**
*	check that the given sample is within the right bit depth
*/
void checkBitDepth(int sample, int bitDepth){
	int upper = power(2, bitDepth -1) -1;
	int lower = -1*(power(2, bitDepth -1));

	if((sample < lower)||(sample > upper)){ /* check if within bitDepth bounds */
		fprintf(stderr, "Error: found sample beyond excepted bit depth limits %d\n", sample);
		exit(EXIT_FAILURE);
	}
}

/*==================== Byte Stuff ====================*/
/**
*	Read number of bytes as an int
*/
int bytesToInt(FILE* stream, int numBytes){
	char buffer[numBytes];
	if(fread(buffer, 1, numBytes, stream));
	if(feof(stream)){
		fprintf(stderr, "Error: found end of file before was specified\n");
		exit(EXIT_FAILURE);
	}
	int num = flipEndianness(buffer, numBytes);
	return num;
}

/**
*	Convert int to binary
*/
void intToBytes(FILE *stream, int num, int bytes){
	int i;
	for(i = (bytes - 1); i >= 0; i--){
		fputc((num >> (i*8)) & 0xFF, stream);
	}
}

/**
*	Read number of bytes as a long
*/
unsigned long floatToLong(FILE* stream){
	unsigned char buffer[10];
	fread(buffer, 1,  10, stream);
	if(feof(stream)){
		fprintf(stderr, "Error: found end of file before was specified\n");
		exit(EXIT_FAILURE);
	}
	unsigned long num = ConvertFloat(buffer);
	return num;
}

/**
*	Flip bytes to opposing endianness
*/
int flipEndianness(char* buffer, int numBytes){
	char* flip = calloc(1, 4);
	int i;
	for(i = 0; i < numBytes; i++){
		flip[i] =  buffer[numBytes -i -1];
	}
	int val = *((int*)flip);

	free(flip);
	return val;
}

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

   FlipLong((unsigned long*)(buffer+2));

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

	/* ************************* StoreLong() ******************************
	 * Fools the compiler into storing a long into a char array.
	 ******************************************************************** */

	void StoreLong(unsigned long val, unsigned long * ptr)
	{
	   *ptr = val;
	}

	/* ************************** StoreFloat() ******************************
	 * Converts an unsigned long to 80 bit IEEE Standard 754 floating point
	 * number.
	 ********************************************************************** */

	void StoreFloat(unsigned char * buffer, unsigned long value)
	{
	   unsigned long exp;
	   unsigned char i;

	   memset(buffer, 0, 10);

	   exp = value;
	   exp >>= 1;
	   for (i=0; i<32; i++) { exp>>= 1;
	      if (!exp) break;
	   }
	   *(buffer+1) = i;

	   for (i=32; i; i--){
	      if (value & 0x80000000) break;
	      value <<= 1;
		}
		StoreLong(value, buffer+2);
		FlipLong((unsigned long *)(buffer+2));
	} 


/*======================================================================**
**																		**
**							Writing Functions							**	
**																		**
**======================================================================*/
/**
*	Write an Aiff file from a CS229 file
*/
void writeAiff(soundfile_t *from, FILE *to){
	int bytes = from->bitDepth/8;
	int blockSize = 0;
	int commSize = 18;
	int ssndSize = (from->samples * from->channels * bytes) + 8;
	if(ssndSize%2){
		ssndSize++;
		blockSize = 1;
	}

	fwrite("FORM", 4, 1, to); /* write FORM */
	int fileSize = commSize + ssndSize + 20;
	intToBytes(to, fileSize, 4); /* write remaining bytes in file */
	fwrite("AIFF", 4, 1, to);/* write AIFF*/
	
	fwrite("COMM", 4, 1, to); /* write COMM */
	intToBytes(to, commSize, 4); /* bytes left in chunk */
	intToBytes(to, from->channels, 2); /* NumChannels */
	intToBytes(to, from->samples, 4); /* NumSampleFrames */
	intToBytes(to, from->bitDepth, 2); /* SampleSize */
	char sampleRate[10]; /* SampleRate */
	StoreFloat(sampleRate, from->sampleRate); /* extended precision floating point */
	fwrite(sampleRate, 10, 1, to);
	
	fwrite("SSND", 4, 1, to); /* write SSND */
	intToBytes(to, ssndSize, 4); /* bytes left in chunk */
	intToBytes(to, 0, 4); /* Offset */
	intToBytes(to, blockSize, 4); /* BlockSize */
	int i;
	int j;
	for(i= 0; i < from->samples; i++) { /* release samples */
		for(j = 0; j < from->channels; j++) {
			intToBytes(to, from->sampleData[(i*from->channels)+j], (from->bitDepth)/8);
		}
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