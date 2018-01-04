/* Author: Seth George */

#ifndef CS229UTIL_H
#define CS229UTIL_H

#include "sndutil.h"

void parseCs229(soundfile_t *snd);
void parseHeader(soundfile_t *snd);
int parseKeywordValue(FILE *stream, int isSamples);
void parseNewLine(int isComment, FILE *stream);
void parseData(soundfile_t *snd);

void writeCs229(soundfile_t *from, FILE* to);

#endif