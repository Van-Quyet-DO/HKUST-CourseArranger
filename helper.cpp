#include "helper.h"
using namespace std;
int convert_day(const char* str){   // Since the data downloaded from websites is consistent, 
                                                      //we don't need to check other cases
    if(str[0] == 'M' and str[1] == 'o'){          // Monday
        return 0;
    }
    else if(str[0] == 'T' and str[1] == 'u'){   // Tuesday
        return 1;
    }
    else if(str[0] == 'W' and str[1] == 'e'){  // Wednesday
        return 2;
    }
    else if(str[0] == 'T' and str[1] == 'h'){   // Thursday
        return 3;
    }
    else if(str[0] == 'F' and str[1] == 'r'){   // Friday
        return 4;
    }
    else if(str[0] == 'S' and str[1] == 'a'){   // Saturday
        return 5;
    }
    else return -1;
}

int len(const char* str){ // May be not cover edge cases, but it is enough for our use
    int length = 0;
    while(str[length] != '\0')
        ++length;
    return length;
}

int convert_session(const char* str){
    // This implementation accept the input as Tu 16:00 - 19:00 and Tu 03:00PM - 07:00PM
    int hh = 10*(str[0]-'0')+str[1]-'0';          // convert string 'ab' to number ab
    int mm = 10*(str[3]-'0')+str[4]-'0';  
    if(str[5] == 'P' && hh < 12) hh += 12; // check it is AM or PM
    return (hh - 9)*2 + (mm+10)/30;         // normally mm is 20 or 50, so we round it to 30 or 60 respectively
}
string capitalize(string input){
    for(int i = 0; i < input.size(); ++i){
        if(input[i] >= 'a' && input[i] <= 'z'){
            input[i] = input[i] + 'A' - 'a';
        }
    }
    return input;
}

int pow(int a, int b){
    if(a == 0) return 0;
    int out = 1;
    for(int i = 0; i < b; ++i){
        out *= a;
    }
    return out;
}
