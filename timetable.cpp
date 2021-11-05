#include "timetable.h"
#include "helper.h"
#include <iomanip>
using namespace std;

int Timetable::count_table = 0;    // initialize static data member
Timetable::Timetable(){
    count_table += 1;
    for(int i = 0; i < 6; ++i){
        for(int j = 0; j < 28; ++j){
            table[i][j] = nullptr;      // make all timeslots empty
        }
    }
    num_credits = 0;

}
Timetable::~Timetable(){
    count_table -= 1;
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 28; ++j)
            delete table[i][j]; // if may cause double deallocation, but it fits to our purpose
                                      // as all the tables sharing Class* will be deleted simutaneously
}


bool Timetable::add_class(const Class* c, string course_name){
    // Setup the key to add
    string title = course_name + " " + c->get_type();

    // Check if the class section has existed in the table or not
    for(int i = 0; i < 6; ++i){
        for(int j = 0; j < 28; ++j){
            if(table[i][j] != nullptr && table[i][j]->get_course_name() == c->get_course_name()
                                      && table[i][j]->get_type() == c->get_type())
                return false;
        }
    }

    // If not add yet, check if we can add
    bool can_add = true;
    const Session* temp = c->get_time();
    for(int ses = 0; ses < 2; ++ses, ++temp)
        if(!(temp->is_empty())){
            for(int i = temp->get_start(); i < temp->get_finish(); ++i){
                can_add = can_add && (table[temp->get_weekday()][i] == nullptr); // check timeslot is occupied yet
            }
        }

    // If we can add, let's add!
    if(can_add){ 
        string str = course_name + " " + c->get_type() + to_string(c->get_id());
        cout << "Adding " << str << " to timetable!" << endl;
        temp = c->get_time();
        for(int ses = 0; ses < 2; ++ses, ++temp)
            if(!(temp->is_empty())){
                for(int i = temp->get_start(); i < temp->get_finish(); ++i)
                    table[temp->get_weekday()][i] = new Class(*c);
        }
    }
    return can_add;
}

bool Timetable::add_course(const Course* c){
    bool can_add = false;
    if(c->lecture.size() == 0){ // Like LANG courses, there is no Lecture within
        for(int j = 0; j < c->tutorial.size(); ++j)
            if(add_class(c->tutorial[j], c->course_name)){
                num_credits += c->credits;
                can_add = true;
                break;
            }
    }
    else if(c->tutorial.size() == 0){ // Like some ACCT courses, there is no Tutorial within
        for(int j = 0; j < c->lecture.size(); ++j)
            if(add_class(c->lecture[j], c->course_name)){
                num_credits += c->credits;
                can_add = true;
                break;
            }
    }
    else{ // Otherwise
        for(int i = 0; i < c->lecture.size(); ++i){
            if(add_class(c->lecture[i], c->course_name)){ // if we can add lecture, we should add the corresponding tutorial class
                for(int j = 0; j < c->tutorial.size(); ++j)
                    if(c->tutorial[j]->get_id() == c->lecture[i]->get_id() &&
                        add_class(c->tutorial[j], c->course_name)){
                            num_credits += c->credits;
                            can_add = true;
                            break;
                        }
                if(can_add) break;
            }
        }
    }
    if(!can_add) cout << "Cannot add " << c->course_name << " because of time conflict!\n";
    return can_add;
}

void Timetable::add_time_constraint(int d, int s){
    table[d][s] = new Class("Occupied",'X',0,0);

}
void Timetable::remove_time_constraint(int d, int s){
    if(table[d][s]!=nullptr){
        delete table[d][s];
        table[d][s]=nullptr;
    }
}

bool Timetable::is_time_constraint(int d,int s){
    return table[d][s] != nullptr;
}

Timetable* Timetable::print(){
    // for checking algorithm, most used in CLI
    string day[] = {"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
    cout << "\n\t\t Timetable: Total number of credits: " << num_credits << '\n'<< endl;
        int width = 13;
        for(int t =-1; t < 27; ++t){
            if(t > -1) cout << setw(2) << 9+t/2 <<':'<< t%2 *3 << '0'; // minor thing
            else cout << setw(7);

            for(int d = 0; d < 6; ++d){
                cout  << "||"<< setw(width);
                if(t == -1) cout <<day[d]+ "  ";
                else if(table[d][t]!=nullptr) cout << table[d][t]->get_course_name();
                else cout<<"";
            }
            cout << '\n' << setfill('=') << setw(7+6*(width+2))<< '\n';
            cout << setfill(' ') ;
        }

    // generate a Timetable for displaying in GUI
    Timetable* tempTimetable = new Timetable();
    tempTimetable->num_credits=num_credits;
    for(int i = 0; i < 6; ++i){
        for(int j = 0; j < 28; ++j){
            tempTimetable->table[i][j]= table[i][j];
        }
    }
    return tempTimetable;


}
bool Timetable::fit(Course** input, int remain, bool need_print, bool take_one){
    // No course left, of course fittable
    if(remain <= 0) return true;

    // Setup
    bool out = false;
    Course* c = *input;
    cout << "Fitting "<< c->course_name << " while remaining " << remain-1 <<" courses" << endl;

    if(c->lecture.size() == 0){ // Like LANG courses, there is no Lecture within
        for(int j = 0; j < c->tutorial.size(); ++j)
            if(add_class(c->tutorial[j], c->course_name)){
                num_credits += c->credits;
                if(fit(input+1,remain-1,need_print,take_one)){ // call itself to check for remaining courses.
                    out = true;
                    if(remain == 1 and need_print and can_export()){
                        Timetable* tempTimeTable = print();
                        emit newTimeTable(tempTimeTable);
                    }
                }
            if(out and take_one) break; // if successfully add all courses and only want to take one (for visual verification), 
                                                       // break to not remove the course and other loops for current course
            else remove_course(c);        // if we don't remove, it may affect the recursion
            }
    }
    else if(c->tutorial.size() == 0){ // Like ACCT courses, there is no Tutorial within
        for(int j = 0; j < c->lecture.size(); ++j)
            if(add_class(c->lecture[j], c->course_name)){
                num_credits += c->credits;
                if(fit(input+1,remain-1,need_print,take_one)){
                    out  = true;
                    if(remain == 1 and need_print and can_export()) {
                        Timetable* tempTimeTable=print();
                        emit newTimeTable(tempTimeTable);
                    }
                }
            if(out and take_one) break;
            else remove_course(c);
            }
    }
    else{ // Otherwise
        for(int i = 0; i < c->lecture.size(); ++i){
            if(add_class(c->lecture[i], c->course_name)){ // if we can add lecture, we should add the corresponding tutorial class
                num_credits += c->credits;
                for(int j = 0; j < c->tutorial.size(); ++j){
                    if((c->tutorial[j]->get_id() == c->lecture[i]->get_id()
                        || c->tutorial[j]->get_type() == 'A')      // Do not require match Lab with Lecture!
                        && add_class(c->tutorial[j], c->course_name)){
                            if(fit(input+1,remain-1,need_print,take_one)){
                                out  = true;
                                if(remain == 1 and need_print and can_export()) {
                                    Timetable* tempTimeTable=print();
                                    emit newTimeTable(tempTimeTable);
                                }
                            }
                    }
                    if(out and take_one) break;
                    else{// string str = course_name + " " + c->get_type() + to_string(c->get_id()), the same as we add
                        string str = c->course_name+" "+c->tutorial[j]->get_type()+ to_string(c->tutorial[j]->get_id());
                        remove_class(str);
                    }
                }
            }
            if(out and take_one) break;
            else remove_course(c);
        }
    }
    remove_course(c); // Without it, table still keeps all courses, then the check in max_credit will be affected
    return out;
}



bool Timetable::max_credits(vector<Course*> sorted_courselist){
    // extract a subset of courselist, then check by fit(), the runtime is O(2^(courselist.size()))
    // fact: for practical use, no user want/should register 10 courses

    vector<Course*> desire; // will store the courselist having valid schedule and maximum credits
    for(int bit = pow(2,sorted_courselist.size())-1; bit > -1; --bit){  // run through all cases
        vector<Course*> temp = extract(sorted_courselist,bit);       // extract the corresponding list
        if(fit(&temp[0],temp.size(),false,true) and         // if find a valid schedule (don't need to print)
            total_credits(desire) < total_credits(temp)){   // with more credits
            desire = temp;                                              // then update "desire"
        }
        clean_keep_constraint(); // play safe
    }

    // this line is for checking in CLI
    cout << "\n\n\n\t\tThe timetable with the largest number of credits is:\n\n\n";
    fit(&desire[0],desire.size(),true, false);  // I want to only print one schedule, but for suggestion I decide to print all
    if(total_credits(desire) > 0)                   // if desire is empty, then we don't need to emit a table in GUI
        return true;
    else return false;

}

void Timetable::clean_all(){
    for(int i = 0; i < 6; ++i){
        for(int j = 0; j < 28; ++j){
                if(table[i][j] != nullptr){
                    delete table[i][j];
                    table[i][j] = nullptr;
                }
        }
    }
    num_credits = 0;
}

void Timetable::clean_keep_constraint(){
    string key = "Occupied";
    for(int i = 0; i < 6; ++i){
        for(int j = 0; j < 28; ++j){
            if(table[i][j]!=nullptr && table[i][j]->get_course_name() != key){
                delete table[i][j];
                table[i][j] = nullptr;
            }
        }
    }
    num_credits = 0 ;
}

void Timetable::remove_course(const Course* c){
    string title = c->course_name;
    bool exist = false;
    for(int i = 0; i < 6; ++i){
        for(int j = 0; j < 28; ++j){
            if(table[i][j] != nullptr && table[i][j]->get_course_name() == title){
                delete table[i][j];
                table[i][j] = nullptr;
                exist = true;
            }
        }
    }
    if(exist) num_credits -= c->credits;
}
void Timetable::remove_class(string title){
    bool exist = false;
    for(int i = 0; i < 6; ++i){
        for(int j = 0; j < 28; ++j){
            if(table[i][j] != nullptr && table[i][j]->get_class_name() == title){
                delete table[i][j];
                table[i][j] = nullptr;
                exist = true;
            }
        }
    }
    if(exist) // for checking purpose in CLI
        cout<<"Remove"<<title<<endl;
}
