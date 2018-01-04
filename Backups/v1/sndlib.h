/* Author: Seth George */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 256
 
typedef struct {
	FILE* fp; /* the file being read */
	char name[MAX_NAME_LENGTH]; /* file location */
	char format[6]; /* CS229 or AIFF */
	int samples; /* number of samples */
	unsigned long sampleRate; /* samples per second */
	int bitDepth; /* 8, 16, or 32 */
	int channels; /* up to 32 channels */
	float duration; /* samples/sampleRate */
	int offset; /* bytes till first sample */
	int blockSize; /* size of a block that the sample data is aligned to */
} soundfile_t;

/* General Code Functions	*/
void initSoundfile(soundfile_t *snd);
void keywordsAreSet(soundfile_t *snd);
int power(int num, int exp);
void printSndInfo(soundfile_t *snd);
void setFormat(soundfile_t *snd);

/* Parsing C229 Functions */
void parseCs229(soundfile_t *snd);
void parseHeader(soundfile_t *snd);
int parseKeywordValue(FILE *fp);
void parseNewLine(int isComment, FILE *fp);
void parseData(soundfile_t *snd);
int parseSampleValue(soundfile_t *snd, int numSamples);

/* Parsing AIFF Functions */
void parseAiff(soundfile_t *snd);
int parseComm(soundfile_t *snd, int remaining);
int parseSsnd(soundfile_t *snd);

/* Byte Stuff */
int bytesToInt(soundfile_t *snd, int numBytes);
unsigned long floatToLong(soundfile_t *snd);
int flipEndianness(char* buffer, int numBytes);
void FlipLong(unsigned char * ptr);
unsigned long FetchLong(unsigned long * ptr);
unsigned long ConvertFloat(unsigned char * buffer);

/* Writing Functions */
writeAiff(soundfile_t *from, File *to);
writeCs229(soundfile_t *from, FILE* to);