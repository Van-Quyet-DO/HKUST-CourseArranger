/* "Course" objects store almost information of courses, 
 * including all non-TBA-time Lecture sections and Tutorial + Lab sections 
 * (if a user wants to enrol in those classes, they don't overlap to any other classes)
 * Some global functions aid the implementation of member functions such as fit() and max_credit() in "Timetable" class
 */

#ifndef COURSE_H_
#define COURSE_H_

#include "class.h"
#include <vector>
#include <QFile>
using namespace std;

class Course{
    friend class Timetable;  // That benefits the implementation of Timetable class
                                        // There is no mutator since each "Course" object is completely built in constructor
    string course_name;
    int credits;
    vector<Class*> lecture; // Using pointer for efficiency
    vector<Class*> tutorial; // Using vector as we don't know how many sections a course has at the beginning

public:
    // Constructor and Destructor
    Course(string coursename);
    ~Course();

    // Accessors
    void print();                            // for debugging purpose
    bool is_empty();
    int get_num_classes() const; // for sorting
    int get_credits() const {return credits;}
};

vector<Course*> sort_by_num_classes(vector<Course*>);   // Sort course lists with ascending number of sections, return the sorted copy
                                                                                             // accelerate fit() performance in almost cases
vector<Course*> extract(vector<Course*>, int binary);           // Take a subset of inputed courses
int total_credits(vector<Course*>);                                       // Total credits of a "Course" list

#endif // COURSE_H_
