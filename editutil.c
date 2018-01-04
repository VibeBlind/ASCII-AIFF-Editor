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
	int avalible = (COLS-32)/2;
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
void printSideMenu(soundfile_t *snd, int mark, int inBuffer, int isModified){
	int i;
	for(i = 0; i < COLS; i++){
		mvprintw(1, i, "="); /* top === border thing */
	}
	
	int hrs, mins;
	float secs = ((float)snd->samples)/snd->sampleRate;
	hrs = secs/3600;
	secs -= hrs*3600;
	mins = secs/60;
	secs -= mins*60;
	
	/* beginning of the side info */
	mvprintw(2, COLS-20, " Sample Rate: %d", snd->sampleRate);
	mvprintw(3, COLS-20, " Bit Depth: %d", snd->bitDepth);
	mvprintw(4, COLS-20, " Channels: %d", snd->channels);
	mvprintw(5, COLS-20, " Samples: %d         ", snd->samples);
	mvprintw(6, COLS-20, " Length: %d:%d:%.2f   ", hrs, mins, secs);
	mvprintw(7, COLS-21, "====================="); /* extra in case of odd rounding */
	if(snd->samples <= 0) {
		mvprintw(8, COLS-20, "           ");
		mvprintw(9, COLS-20, "                   "); /* overwriting with spaces makes so I don't need to refresh */
		mvprintw(10, COLS-20, "                   ");
		mvprintw(LINES-2, COLS-20, "                   ");
	} else if(mark != -1){ /* marked index */
		mvprintw(8, COLS-20, "  m: unmark");
		mvprintw(9, COLS-20, "  c: copy");
		mvprintw(10, COLS-20, "  x: cut");
		mvprintw(LINES-2, COLS-20, "  Marked: %d", mark);
	} else {
		mvprintw(8, COLS-20, "  m: mark  ");
		mvprintw(9, COLS-20, "                   "); /* overwriting with spaces makes so I don't need to refresh */
		mvprintw(10, COLS-20, "                   ");
		mvprintw(LINES-2, COLS-20, "                   ");
	}
	
	if(snd->samples <= 0) {
		mvprintw(11, COLS-20, "                   ");
		mvprintw(12, COLS-20, "                   ");
		mvprintw(LINES-1, COLS-20, "                   ");
	} else if(inBuffer){ /* if stuff is the buffer, we can also use it as a flag */
		mvprintw(11, COLS-20, "  ^: insert before");
		mvprintw(12, COLS-20, "  v: insert after");
		mvprintw(LINES-1, COLS-20, " Buffered: %d", inBuffer/snd->channels);
	} else {
		mvprintw(11, COLS-20, "                   ");
		mvprintw(12, COLS-20, "                   ");
		mvprintw(LINES-1, COLS-20, "                   ");
	}
	
	if(isModified){
		mvprintw(13, COLS-20, "  s: save");
	} else {
		mvprintw(13, COLS-20, "                   ");
	}
	
	mvprintw(14, COLS-20, "  q: quit");
	mvprintw(15, COLS-20, "");
	mvprintw(16, COLS-20, " Movement:");
	mvprintw(17, COLS-20, "  up/down");
	mvprintw(18, COLS-20, "  pgup/pgdn");
	mvprintw(19, COLS-20, "  g: goto");
	
	for(i = 20; i < LINES-3; i++){ /* fill in the every below with empty space */
		mvprintw(i, COLS-20, "                   ");
	}
	mvprintw(LINES-3, COLS-21, "====================="); /* extra in case of odd rounding */
}
