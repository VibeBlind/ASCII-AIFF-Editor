/* Author: Seth George */

#include "aiffutil.h"
#include "cs229util.h"
#include <ncurses.h>

int main(int argc, char* argv[]){
	if(strcmp(argv[1], "-h") == 0){ /* if the arg is -h, display help screen */
		fprintf(stderr, "\nSndedit is an audio file editor based on the ncurses library.\n");
		fprintf(stderr, "The executable takes a single argument\n");
		fprintf(stderr, "where the argument is the pathname of an audio file (either AIFF or CS229 format) to be edited. \n");
		fprintf(stderr, "If the audio file cannot be opened, or is not a valid file, or some other error condition occurs,\n");
		fprintf(stderr, "sndedit should print an appropriate message to standard error, and terminate.\n");
		fprintf(stderr, "Otherwise, sndedit should display an interactive screen (using ncurses).\n");
		fprintf(stderr, "The top 2 lines are fixed with the first line used to display the pathname and format\n");
		fprintf(stderr, "of the file being edited to display the pathname and format of the file being edited.\n");
		fprintf(stderr, "The rightmost 20 columns are used for the sound information and menu.\n");
		fprintf(stderr, "The leftmost 9 columns are used for the sample numbers.\n");
		fprintf(stderr, "The remaining columns are used to display sound data, in a similar manner as sndshow.\n");
		fprintf(stderr, "Most of the time, the cursor is somewhere in the center | column,\n");
		fprintf(stderr, "and moves up and down when the arrow keys are pressed.\n");
		fprintf(stderr, "The left portion of the screen (with the sound data) scroll up and down\n");
		fprintf(stderr, "when the cursor reaches the edge of the window, and whenever the page up and page down keys are pressed.\n");
		fprintf(stderr, "Additionally, the following supported keystrokes, and are not case sensitive:\n");
		fprintf(stderr, "\t'g' (goto): somewhere in the menu window, prompt the user for a sample number,\n");
		fprintf(stderr, "\t\tand then restore the window. If a legal sample numer is entered, scroll the sound data window\n");
		fprintf(stderr, "\t\tsuch that the cursor is on the specified sample.\n");
		fprintf(stderr, "\t'm' (mark): If no mark is set, then use the current sample number as the marked sample\n");
		fprintf(stderr, "\t\totherwise, clear the marked sample. When a sample is marked, all samples between the\n");
		fprintf(stderr, "\t\tmarked sample and the cursor should be displayed in reverse video.\n");
		fprintf(stderr, "\t\tThese are the samples that are selected for the copy and cut operations, below.\n");
		fprintf(stderr, "\t\tIf there is a marked sample, then the bottom right corner of the display should show which\n");
		fprintf(stderr, "\t\tsample number is marked, otherwise, that line should not appear in the display.\n");
		fprintf(stderr, "\t'c' (copy): If no samples are marked, then this menu item should not be available\n");
		fprintf(stderr, "\t\tOtherwise, all marked samples are copied into a buffer.\n");
		fprintf(stderr, "\t\tIf one or more samples are stored in the buffer\n");
		fprintf(stderr, "\t\tthen the bottom right corner of the display should show the current number of samples in the buffer.\n");
		fprintf(stderr, "\t'x' (cut): Just like copy, except the selected samples are also removed from the sound data.\n");
		fprintf(stderr, "\t'^' (paste): If no samples are in the buffer, then this menu item should not be available.\n");
		fprintf(stderr, "\t\tOtherwise, copy the contents of the buffer into the sample data,\n");
		fprintf(stderr, "\t\tright before wherever the cursor is currently positioned.\n");
		fprintf(stderr, "\t'v' (paste): Just like the other paste keystroke, except\n");
		fprintf(stderr, "\t\tthe buffer is copied right after wherever the cursor is currently positioned.\n");
		fprintf(stderr, "\t's' (save): If the sound data has not been modified, then this menu item should not be available.\n");
		fprintf(stderr, "\t\tOtherwise, this causes any changes to the sound data to be saved back to the original file.\n");
		fprintf(stderr, "\t'q' (quit): Revert the screen back to normal, and quit.\n");
		fprintf(stderr, "Note that the upper right of the display, which shows the information about the sound data,\n");
		fprintf(stderr, "should be updated whenever the sound data is modified.\n");
		return 0;
	}
	
	soundfile_t snd;
	initSoundfile(&snd);
	
	snd.stream = fopen(argv[1], "r"); /* argv[1] should be a file name to open */
	strcpy(snd.name, argv[1]); /* set name to argument */
		
	setFormat(&snd); /* checks if is valid file pointer too*/
	
	if(strcmp(snd.format, "CS229") == 0){
		parseCs229(&snd);
	} else {
		parseAiff(&snd);
	}
	
	fclose(snd.stream); /* close file after parsing */
	
	/************************* NCURSES BEGINNING *************************/
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	
	char *title = calloc((strlen(snd.name)+8), sizeof(char)); /* make the top title string */
	strcat(title, snd.name); /* because they are all set to null, "end of string" will be title[0] */
	if(strcmp(snd.format, "CS229") == 0){
		strcat(title, "(CS229)");
	} else {
		strcat(title, "(AIFF)");
	}
	
 	int button = 0;
	int x = (COLS/2) - 20;
	int y = 2; /* start at the top */
	
	/************************* SOUND DATA *************************/
	int i;
	int j;
	int startSample = 0;
	int numDisplayed = snd.samples * snd.channels; /* number to things to display, all the channels of the samples */
	int allDisplayed[numDisplayed];
	char *buffer = calloc(9, 1); /* first part of the bar is 9 chars long */
	while((button != 'q')&&(button != 'Q')){
		for(i = 0; i < LINES - 2; i++){ /* put the sample number in the first 9 chars */
			move(i + 2, 0);
			sprintf(buffer, "%d", startSample + i);
			for(j = 0; j < (9 - strlen(buffer)); j ++) { /* the first chars are spaces, then the number */
				printw(" ");
			}
			printw("%s|", buffer); /* the number should be "right justified" */
		}
		
		/************************* INFO FOR USER *************************/
		if((COLS < MIN_COLS) || (LINES < MIN_ROWS)){
			fprintf(stderr, "Error: window was too small\n");
			exit(EXIT_FAILURE);
		}
		
		x = (COLS-20)/2;
		mvprintw(0, (COLS - strlen(title))/2, "%s", title); /* print out title */
		
		printSideMenu(&snd, COLS, LINES);
		move(y, x); /* move cursor back */
		
		/************************* KEYSTORKES *************************/
		button = getch(); /* get next button press */
		switch(button){
			case KEY_UP: /* arrow up */
				if(y > 2){ /* move cursor up */
					y--;
					move(y, x);
				} else if(startSample > 0){ /* scroll page up */
					startSample--;
				}
				break;
				
			case KEY_DOWN: /* arrow down */
				if(y < LINES -1){
					y++;
					move(y, x);
				} else if(startSample < snd.samples){ /* scroll page down */
					startSample++;
				}
				break;
				
			case KEY_PPAGE: /* page up */
				if(startSample < snd.samples){ /* scroll page up */
					startSample++;
				}
				break;

			case KEY_NPAGE: /* page down */
				if(startSample > 0){ /* scroll page up */
					startSample--;
				}
				break;
				
			case 'g': /* goto */
			case 'G':
				break;
			case 'm': /* mark */
			case 'M':
				break;
			case 'c': /* copy */
			case 'C':
				break;
			case 'x': /* cut */
			case 'X':
				break;
			case '^': /* paste */
			case '6': /* shift + 6 = ^, so "lowercase" ^ is 6... ugh*/
				break;
			case 'v': /* paste */
			case 'V':
				break;
			case 's': /* save */
			case 'S':
				break;
			default: /* everything else */
				break;
		}
		clear();
		refresh();
	}
	
	clrtoeol();
	refresh();
	endwin();
	
	return 0;
}