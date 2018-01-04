/* Author: Seth George */

#include "editutil.h"
#include "sndutil.h"
#include <ncurses.h>

/**
*	Prints the sample data
*/
void printBarCurses(int value, int bitDepth){
	int max = power(2, bitDepth -1)-1; /* maximum possible value for bit depth */
	int sign = 1;
	if(value < 0) {
		sign = -1;
	}
	
	float ratio = (float)value*sign / max; /* ratio equals value / max value, rounding up as .5 */
	int avalible = (COLS - 12)/2;
	int dashes = ratio * avalible + 0.5f;
	
	int i;
	if(value < 0){ /* if negative */
		for(i = 0; i < avalible - dashes; i++){
			printw(" ");
		}
		for(i = 0; i < dashes; i++){
			printw("-");
		}
		printw("|");
		for(i = 0; i < avalible; i++){
			printw(" ");
		}
	} else { /* else if positive */
		for(i = 0; i < avalible; i++){
			printw(" ");
		}
		printw("|");
		for(i = 0; i < dashes; i++){
			printw("-");
		}
		for(i = 0; i < avalible - dashes; i++){
			printw(" ");
		}
	}
	printw("|"); /* pipe at the end of the bar */
}

/**
*	Print the side bar in sndedit
*/
void printSideMenu(soundfile_t *snd, int COLS, int numRows){
	int i;
	for(i = 0; i < COLS; i++){
		mvprintw(1, i, "="); /* top === border thing */
	}
	
	int mark = -1, canPaste = 0, isModified = 0; /* Flags */
	int hrs, mins;
	float secs = snd->duration;
	hrs = secs/3600;
	secs -= hrs*3600;
	mins = secs/60;
	secs -= mins*60;
	
	/* beginning of the side info */
	mvprintw(2, COLS-20, " Sample Rate: %d", snd->sampleRate);
	mvprintw(3, COLS-20, " Bit Depth: %d", snd->bitDepth);
	mvprintw(4, COLS-20, " Channels: %d", snd->channels);
	mvprintw(5, COLS-20, " Samples: %d", snd->samples);
	mvprintw(6, COLS-20, " Length: %d:%d:%.2f", hrs, mins, secs);
	mvprintw(7, COLS-20, "====================");
	
	if(mark != -1){
		mvprintw(8, COLS-20, "  m: unmark");
		mvprintw(9, COLS-20, "  c: copy");
		mvprintw(10, COLS-20, "  x: cut");
		mvprintw(numRows-2, COLS-20, "  Marked: %d", mark);
	} else {
		mvprintw(8, COLS-20, " m: mark");
		mvprintw(9, COLS-20, "");
		mvprintw(10, COLS-20, "");
		mvprintw(numRows-2, COLS-20, "");
	}
	
	if(canPaste){
		mvprintw(11, COLS-20, "  ^: insert before");
		mvprintw(12, COLS-20, "  v: insert after");
	} else {
		mvprintw(11, COLS-20, "");
		mvprintw(12, COLS-20, "");
	}
	
	if(isModified){
		mvprintw(13, COLS-20, "  s: save");
	} else {
		mvprintw(13, COLS-20, "");
	}
	
	mvprintw(14, COLS-20, "  q: quit");
	mvprintw(15, COLS-20, "");
	mvprintw(16, COLS-20, " Movement:");
	mvprintw(17, COLS-20, "  up/down");
	mvprintw(18, COLS-20, "  pgup/pgdn");
	mvprintw(19, COLS-20, "  g: goto");
	
	for(i = 20; i < numRows-3; i++){ /* fill in the every below with empty space */
		mvprintw(i, COLS-20, "");
	}
	
	mvprintw(numRows-3, COLS-20, "====================");
	mvprintw(numRows-1, COLS-20, " Buffered: bytes in buf");
}
