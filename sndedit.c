/* Author: Seth George */

#include "aiffutil.h"
#include "cs229util.h"
#include <ncurses.h>

int main(int argc, char* argv[]) {
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
	
	int button = 0, mark = -1, inBuffer = 0, isModified = 0, reverseVideo = 0, cols = COLS, rows = LINES; /* Flags */
	int x = (COLS-12)/2, y = 2; /* x = middle of sndData, y = top right below title border */
	int i, j, k;
	int startSample = 0;
	int cursorSample = 0;
	char *sampleIndex = calloc(9, 1); /* first part of the bar is 9 chars long */
	int *buffer = malloc(1);
	while((button != 'q')&&(button != 'Q')){
		/************************* SCREEN CHECK *************************/
		if((COLS < MIN_COLS) || (LINES < MIN_ROWS)){
			fprintf(stderr, "Error: window was too small\n");
			exit(EXIT_FAILURE);
		}
		
		if(COLS != cols || LINES != rows){ /* update screen if screen width changes */
			clear();
			refresh();
			cols = COLS;
			rows = LINES;
		}
		
		/************************* INFO FOR USER *************************/
		mvprintw(0, (COLS - strlen(title))/2, "%s", title); /* print out title */
		printSideMenu(&snd, mark, inBuffer, isModified);
		
		/************************* SOUND DATA *************************/
		int high, low; /* will also be used in copy and cut */
		cursorSample = startSample + (y-2)/snd.channels;
		if(cursorSample < mark){
			low = cursorSample;
			high = mark;
		} else {
			low = mark;
			high = cursorSample;
		}
		
		/* print by a sample at a time */
		for(i = 0; i < LINES - 2; i+= snd.channels){ /* i/channels is the sample being printed relative to the screen*/
			if(mark != -1){ /* if the sample is within cursor and mark */
				if(((startSample + i/snd.channels) >= low) && ((startSample + i/snd.channels) <= high)){
					attron(A_REVERSE); /* reverse video */
				} else {
					attroff(A_REVERSE);/* un-reverse video*/
				}
			}
			
			if((snd.samples > 0) && ((i/snd.channels + startSample) < snd.samples)){ /* if we should print a sample*/
				move(2 + i, 0);
				sprintf(sampleIndex, "%d", startSample + i/snd.channels); 
				
				for(j = 0; j < (9 - strlen(sampleIndex)); j ++) { /* the first chars are spaces, then the number */
					printw(" ");
				}
				
				printw("%s|", sampleIndex); /* the number should be "right justified" */
				printBarCurses(snd.sampleData[i + startSample*snd.channels], snd.bitDepth);
				
				for(j = 1; j < snd.channels; j++){
					if((2+i+j) < LINES){
						mvprintw(2+i+j, 0, "         |"); /* for multiple channels, we don't show a number */
						printBarCurses(snd.sampleData[i + j + startSample*snd.channels] , snd.bitDepth); /* print out the dash bar representation */
					}
				}
			} else { /* if we shouldn't print anything, print blank lines */
				attroff(A_REVERSE);
				int printAtY = 2 + i;
				for(j = 0; j < snd.channels; j++){
					if(printAtY < LINES) {
						move(printAtY, 0);
						for(k = 0; k < COLS-22; k++){
							addch(' ');
						}
						if(COLS%2 == 0){
							addch(' ');
						}
						addch('|');
					}
					printAtY++;
				}
			}
		}
		attroff(A_REVERSE);
		
		/************************* KEYSTROKES *************************/
		x = (COLS-12)/2;
		move(y, x); /* move cursor back */
		button = getch(); /* get next button press */
		mvprintw(0,0, "heyz2");/////////////////////////////////////////////////
		if((snd.samples > 0)&& (button == KEY_UP)){ /* arrow up */
			if(cursorSample > 0){
				if(y > 2){ /* move cursor up */
					y -= snd.channels;
					move(y, x);
				} else if(startSample > 0){ /* scroll page up */
					startSample--;
				}
			}
			
		} else if((snd.samples > 0)&&(button == KEY_DOWN)){ /* arrow down */
			if(cursorSample < (snd.samples - 1)){
				if(y < LINES -1){
					y += snd.channels;
					move(y, x);
				} else if(startSample < (snd.samples - 1)){ /* scroll page down */
					startSample++;
				}
			}
			
		} else if((snd.samples > 0)&&(button == KEY_PPAGE)){ /* page up */
			if(startSample > 0){ /* scroll page up*/
				startSample--;
			}
			
		} else if((snd.samples > 0)&&(button == KEY_NPAGE)){ /* page down */
			if(cursorSample < (snd.samples - 1)){ /* scroll page down */
				startSample++;
			}
			
		} else if((snd.samples > 0)&&(button == 'g'|| button == 'G')){ /* goto */
			char *input = calloc(16, sizeof(char));
			char c = 0;
			int invalid = 0;
			
			mvprintw(LINES-1, 0, "Please enter the sample number you want to go to: ");
			for(i = 0; i < 15; i++){
				c = getch();
				addch(c);
				if(c == '\n') {
					break;
				}
				if(c < '0' || c > '9'){ /* if not a number or zero*/
					invalid = 1;
					break;
				}
				
				input[i] = c;
			}
			
			int test = atoi(input);
			if(!invalid && test < snd.samples){
				startSample = test;
			}
			
			free(input);
			y = 2;
		
		} else if((snd.samples > 0)&&(button == 'm' || button == 'M')){ /* mark */
			if(mark == -1){
				mark = startSample + (y-2)/snd.channels;
			} else {
				mark = -1;
				attroff(A_REVERSE); /* redundant turn off reverse video */
			}
			
		} else if((snd.samples > 0)&&(button == 'c' || button == 'C')){ /* copy */
			if(mark != -1){				
				inBuffer = 0;
				int numSamples = high - low + 1; /* number of samples in buffer */
				free(buffer);
				buffer = malloc(numSamples * snd.channels * sizeof(int));
				for(i = low; i <= high; i++) { /* for each sample */
					for(j = 0; j < snd.channels; j++){ /* add every channel in the sample */
						buffer[inBuffer] = snd.sampleData[i*snd.channels + j];
						inBuffer++;
					}
				}
			}
			
		} else if((snd.samples > 0)&&(button == 'x' || button == 'X')){ /* cut */
			if(mark != -1){
				/* low and high are already declared when checking for reverse video */
				inBuffer = 0;
				int numSamples = high - low + 1; /* number of samples in buffer */
				free(buffer);
				buffer = malloc(numSamples * snd.channels * sizeof(int));
				for(i = low; i <= high; i++) { /* for each sample */					
					for(j = 0; j < snd.channels; j++){ /* add every channel in the sample */
						buffer[inBuffer] = snd.sampleData[i*snd.channels + j];
						inBuffer++;
					}
				}
				int lowArr[1] = {low};
				int highArr[1] = {high};
				dataCut(lowArr, highArr, 1, &snd);
				if(startSample >= snd.samples){
					startSample = snd.samples - 1;
					y = 2;
				}
				mark = -1;
				isModified = 1;
			}
			
		} else if((snd.samples > 0)&&(button == '6' || button == '^')){ /* paste above, shift + 6 is ^, so "lowercase" ^ is 6 */
			if(inBuffer){
				dataCat(cursorSample, buffer, inBuffer, &snd);
				isModified = 1;
			}
			
		} else if((snd.samples > 0)&&(button == 'v' || button == 'V')){ /* paste below */
			if(inBuffer){
				dataCat(cursorSample + 1, buffer, inBuffer, &snd);
				isModified = 1;
			}
			
		} else if(button == 's' || button == 'S'){ /* save */
			if(isModified){
				FILE *fpout = fopen(snd.name, "w"); /* open for writing */
				/* write the converted file */
				if((strcmp(snd.format, "CS229") == 0)){
					writeCs229(&snd, fpout);
				} else {
					writeAiff(&snd, fpout);
				}
				fclose(fpout);
				isModified = 0;
			}
		}
	}
	/* clean up when we're done */
	clrtoeol();
	refresh();
	endwin();
	
	return 0;
}