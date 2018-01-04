Author: Seth George

The README is a brief explanation to the following programs:
sndinfo.c
sndconv.c
sndcat.c
sndcut.c
sndshow.c
cs229util.c and cs229util.h
aiffutil.c and aiffutil.h
sndutil.c and sndutil.c

sndinfo.c:
	Includes aiffutil.h and cs229util.h which include sndutil.h
	
	This program reads a given file or files and prints a list of stats about the file to stdout.
	This program supports passing the file names and other switches as arguments.
	Arguments may be passed as: $ sndinfo [switches] [file] [file] ...
	where brackets indicate optional arguments. For each specified input file,
	If no file names are specified, then sndinfo treats the standard input stream
	as the file, and show a file name of (standard input).
	
	This program supports the following switches, passed as arguments.
		-h: Displays a short help screen to standard error, and terminates cleanly
		-1: Prompt for a file name rather than accepting it as an argument

		
sndconv.c:
	Includes aiffutil.h and cs229util.h which include sndutil.h
	
	This program is read from standard input and writes to standard output.
	I.e., the standard input stream should be treated as the input file,
	and the converted file should be written directly to the standard output stream.
	Any messages (e.g., errors) are written to standard error.
	
	This program supports the following switches, passed as arguments.
		-h: Displays a short help screen to standard error, and then terminates cleanly
		-1: Prompts for a file name rather than accepting it as an argument
		-a: Force output to be AIFF, regardless of the input format
		-c: Force output to be CS229, regardless of the input format
		
		
sndcat.c:
	Includes aiffutil.h and cs229util.h which include sndutil.h
	
	This program reads all sound files passed as arguments, and writes a single sound file
	where the sample data is the concatenation of the sample data in the inputs.
	The resulting sound file are written directly to standard output.
	If no files are passed as arguments, then the standard input stream are treated as the input file.
	Any messages (e.g., errors) are written to standard error.
	
	This program supports the following switches, passed as arguments.
		-h: Displays a short help screen to standard error, and then terminate cleanly.
		-a: Force output to be AIFF
		-c: Force output to be CS229
		
		
sndcut.c:
	Includes aiffutil.h and cs229util.h which include sndutil.h
	
	This program reads a sound file from the standard input stream,
	and writes the sound file to the standard output stream in the same format,
	after removing all samples specified as arguments.
	Any messages (e.g., errors) are written to standard error.
	Specifically, arguments may be passed as: $ sndcut [switches] [low..high] [low..high] ...
	where low..high specifies that all samples numbered between low and high,
	including the samples at low and high themselves, are to be removed.
	
	This program supports the following switches, passed as arguments.
		-h: Display a short help screen to standard error, and then terminate cleanly.
		
		
sndshow.c:
	Includes aiffutil.h and cs229util.h which include sndutil.h
	
	This program reads a sound file from the standard input stream,
	and displays an ASCII art representation of the sample data formatted as follows:
	The first 9 characters are used to display the sample number which are right justified.
	Next comes a pipe character, "|"
	The next x characters are used for plotting negative values.
	Next comes a pipe character that represents the value 0.
	The next x characters are used for plotting positive values.
	The final character is another pipe character.
	The value of x is dictated by the desired total output width; see the program switches.
	For each sample frame, there will be c lines of output, where c is the number of channels.
	The sample number is displayed only for the first line of output for the sample.
	Then, for each channel, a line is drawn using the
	dash character to represent the sample value for that channel.
	A line segment is drawn between 0 and the sample value, then is overlaid x equal-width boxes between
	0 and the maximum sample value, if the sample value is positive,
	or between 0 and the minimum sample value, if the sample value is negative.
	For each box that is half full or more, write a - character, otherwise write a space.
	
	This program supports the following switches, passed as arguments.
		-h: Display a short help screen to standard error, and then terminates cleanly.
		-c c: Show the output only for channel c, for 1 =< c =< #channels.
		-w w: Specify the total output width, in number of characters.
			If not specified, the default is w = 80. Supports values down to w = 20
			Note that w must be even so that the number of characters for representing positive values
			is equal to the number of characters for representing negative values
		-z n: Zoom out by a factor of n. If not specified, the default is n = 1
			The value to plot should be the largest magnitude value over n consecutive samples,
			and the number of lines of output should decrease by about a factor of n
		
		
cs229util:
	This program is for CS229 file specific functions, such as those used to parse and create CS229 files.
	The header file for these functions shares the same name.
	
	
aiffutil:
	This program is for AIFF file specific functions, such as those used to parse and create AIFF files
	Some functions are for bit manipulation are also housed here, such as one that flips endianness
	and ones that convert bytes to an int or an int to bytes.
	The header file for these functions shares the same name.
	
	
sndutil:
	This program holds the general purpose functions, such as exponential function "power"
	and the for initializing the soundfile_t. As some of the more file specific functions call
	these functions, sndutil.h is included in cs229util.h and aiffutil.h. This program also holds
	the basic method used in part1 for sndinfo and sndcov. The header file hold a typedef struct soundfile_t, 
	which holds the stats about the such things as the number of samples, channels, and the sampleRate.
	The header file for these functions shares the same name.
	