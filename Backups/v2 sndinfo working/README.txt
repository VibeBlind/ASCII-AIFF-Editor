Organization and Methods:
	The project only has one header, sndlib.h, and one file for functions being called, sndutil.c.
	This is allows for ease of use and less bulky redundant functions in multiple files,
	because many functions are reused for both sndinfo.c and sndconv.c making individual utility files
	would result in a web of interlaced dependencies, and this is much cleaner, if a bit longer.
	There are comments on the not so intuitive logic and a line describing the purpose of the function,
	as well as a billboard comment denoting where similar functions are grouped (e.g. parsing functions).
	
sndinfo:
	The purpose of sndinfo is to parse through a given file, and return a panel of information about it.
	it works as follows: After prompting and taking in the path to a file to read, it reads the first word of file
	checks for if the file is of an appropriate format, and parses the file accordingly.
	If the file is an invalid format, an error is thrown. Valid formats are CS229 and AIFF.	

	If the format is CS229, parse each line and look for keywords
	collect the values after each keyword disregarding lines starting with #
	if a line doesn't start with # or a keywords, throw an error and exit
	no keywords should be after start data is declared
	channels are read on the same line

	if AIFF parse each the next 4 bytes to get remaining bytes in file
	search for the COMM chunk and gather keyword information
	search for the SSND chunk and gather it block information
	parse the rest of the file
	
sndconv:
	This program converts from CS229 file format AIFF, and vice versa.
	The program reads in the name of the input file, and then read in the name of the output file
	The output file should be written in the opposing file format