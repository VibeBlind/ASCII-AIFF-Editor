/* Author: Seth George */

#ifndef EDITUTIL_H
#define EDITUTIL_H

#include "sndutil.h"
#include <ncurses.h>

void printBarCurses(int value, int bitDepth);
void printSideMenu(soundfile_t *snd, int numCols, int numRows);

#endif