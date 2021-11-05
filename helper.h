/* This file consists of helper functions for the implementation of member functions in other classes:
 * - convert_day(), convert_session() help constructing Session object
 * - capitalize() normalizes the input as the name of courses (comp 2012h -> COMP 2012H)
 * - pow() help to compute total numbers of cases when evaluating the schedule carrying maximal credits
 */


#ifndef FILE_IO_FUNC_H_
#define FILE_IO_FUNC_H_

#define NULL_CHAR '\0'
#define tab '\t'
using namespace std;
#include <string>

int convert_day(const char* str); 		    // convert 2-char string to weekday (e.g "Mo" -> 0, -1 indicates no schedule)
int len(const char* str);                         // length of a "const" string or char array
int convert_session(const char* str);	// convert string carrying time (e.g "07:00PM") to an integer 
                                                            // (w.r.t linear scale: 9:00AM -> 0, 11:00PM -> 28, each slot last 30 mins)
string capitalize(string input);                // capitalize all letter in input string
int pow(int base, int exponent);              // calculate power "exponent" of "base" (for positive number)

#endif // FILE_IO_FUNC_H_
