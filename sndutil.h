/* Author: Seth George */

#ifndef SNDUTIL_H
#define SNDUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 256
#define MIN_COLS 40
#define MIN_ROWS 24
 
typedef struct {
	FILE *stream; /* the file being read */
	char name[MAX_NAME_LENGTH]; /* file location */
	char format[6]; /* CS229 or AIFF */
	int samples; /* number of samples */
	unsigned long sampleRate; /* samples per second */
	int bitDepth; /* 8, 16, or 32 */
	int channels; /* up to 32 channels */
	float duration; /* samples/sampleRate */
	int offset;
	int blockSize;
	int numBytes; /* size of sample data */
	int *sampleData; /* stores the sample data*/
} soundfile_t;

/* General Code Functions */
void printDisplayed(int value, int bitDepth, int w);
void initSoundfile(soundfile_t *snd);
void keywordsAreSet(soundfile_t *snd);
void printSndInfo(soundfile_t *snd);
void printDisplayed(int displayed, int max, int w);
void setFormat(soundfile_t *snd);
void dataCut(int low[], int high[], int numRestrictions, soundfile_t *snd);
void dataCat(int insertSample, int *insertBuffer, int bufferSize, soundfile_t *snd);
int zoomSample(int index, int n, soundfile_t *snd);
int power(int num, int exp);
void basicSndinfo();
void basicSndconv();

#endif