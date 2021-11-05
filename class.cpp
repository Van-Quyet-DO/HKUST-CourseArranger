#include "class.h"
#include "helper.h"

using namespace std;

Session::Session(){
    start = finish = 0;
    weekday = -1;
}

void Session::print(){
    cout << weekday << "\t[" << start << "," << finish << ")\n";
}

void Session::set_value(int s, int f, int w){
    start = s; finish = f; weekday = w;
}

bool Session::is_empty() const {
    return 0 > weekday || weekday > 5;
}

bool Session::overlap(Session s){ // only 3 cases of not overlapping
    return !(	(weekday != s.weekday) || 
                (finish <= s.start) ||
                (start >= s.finish)
            );
    }

Class::Class(string course, char t, int i, int c){
    type = t;
    id = i;
    instructor = "";
    course_name = course;
    credits = c;
}
Class::Class(string course){
    course_name = course;
}
void Class::print(){
    cout << type << id << tab << instructor << endl;
    cout << quota << tab << enrol << tab << avail << tab << wait << endl;
    for(int i = 0; i < 2; ++i){
        time[i].print();
    }
};

void Class::add_session(const char* input){
    if(input[0] == NULL_CHAR) return;                                            // empty string means nothing to add
    if(input[0] == 'T' and input[1] == 'B' and input[2] == 'A') return;     // ignored TBA
    char weekday[5]; char start[8]; char finish[8];                                // Note: "weekday" may be "TuTh"
    sscanf(input,"%s %s %*s %s",weekday, start, finish);                     // scan info in order
    if(len(weekday) <= 2){ // it means the input carries only one session of the section
        if(time[0].is_empty())
            time[0].set_value(convert_session(start),convert_session(finish),convert_day(weekday));
        else
            time[1].set_value(convert_session(start),convert_session(finish),convert_day(weekday));
    }
    else{ // 2 sessions in input, i.e weekdays are merged to 4-char string
        time[0].set_value(convert_session(start),convert_session(finish),convert_day(weekday));
        time[1].set_value(convert_session(start),convert_session(finish),convert_day(weekday+2));
    }
    if(time[0].overlap(time[1])){ // check overlap to merge sessions if necessary
        int b = min(time[0].get_start(),time[1].get_start());
        int e = max(time[0].get_finish(),time[1].get_finish());
        time[0].set_value(b,e,time[0].get_weekday());
        time[1].set_value(0,0,-1);
    }
}

void Class::add_location(string input){
    location = input;
}

void Class::add_instructor(string input){
    instructor += input+"\t";
}

void Class::set_info(string arr[]){
    quota = arr[0]; enrol = arr[1]; avail = arr[2]; wait = arr[3];
}
bool Class::is_empty(){
    return time[0].is_empty() && time[1].is_empty();
}
