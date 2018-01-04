/* Author: Seth George */

#ifndef AIFFUTIL_H
#define AIFFUTIL_H

#include "sndutil.h"

/* Parsing AIFF Functions */
void parseAiff(soundfile_t *snd);
void parseComm(soundfile_t *snd, int chunkSize);
void parseSsn(soundfile_t *snd, int chunkSize, char **storage);
void checkSamples(soundfile_t *snd, char** storage);
void checkBitDepth(int sample, int bitDepth);

/* Byte Stuff */
void intToBytes(FILE *stream, int num, int bytes);
int bytesToInt(FILE* stream, int numBytes);
unsigned long floatToLong(FILE* stream);
int flipEndianness(char *buffer, int numBytes);
void FlipLong(unsigned char *ptr);
unsigned long FetchLong(unsigned long *ptr);
unsigned long ConvertFloat(unsigned char *buffer);

void writeAiff(soundfile_t *from, FILE* to);

#endif