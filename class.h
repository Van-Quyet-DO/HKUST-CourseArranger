/* This header file declares 2 basic classes to build "Course" class
 * "Session" is the numericalized version of real time session, help to store schedule in simple form and control time crash
 * "Class" consists of all data as the name of data member expressed, be used to build "Course" and display info in GUI
 */

#ifndef LECTURE_H_
#define LECTURE_H_

#include <iostream>
#include <string>

class Session{ // Session format is weekday,[start,finish), e.g Fr 09:00AM - 10:20AM -> 4,[0,3)
    int start;
    int finish;
    int weekday;
public:
    Session();
    void print();
    void set_value(int s, int f, int w);
    bool is_empty() const;
    bool overlap(Session);  // For some courses having strange schedule (e.g first half of semester tutorial class is from 9:00PM to 10:PM,
                                        // but for the rest it is from 9:30PM to 10:30PM, so we decide to check if this is the case will merge them to
                                        // a session from 9:00PM to 10:30PM)
    int get_start() const {return start;}
    int get_finish() const {return finish;}
    int get_weekday() const {return weekday;}
};

class Class{            // Refer to Section (Lecture, Tutorial, Lab) like COMP 2012H L1, so can consist of 2 sessions
    // A block of information is all data of a class, may consist of 2 or 3 successive lines
    // First line in HTML
    std::string course_name;
    int credits;
    char type;          // 'L' Lecture or 'T' Tutorial or 'A' for lAb
    int id;                 // ID '1'
    std::string code;  // "(1023)"

    // Second line in HTML (or third line in addition if 'Remark' make a new line)
    Session time[2]; // array of session, at most 2 sessions per week
    std::string instructor;
    std::string location;
    std::string quota, enrol, avail, wait;

public:
    // Constructors
    Class(std::string course,char type, int id, int credit);      // for 4 first data members
    Class(std::string course);                                            // for course_name

    // Mutators
    void add_session(const char*);                                 // The input will be like "Fr 09:00AM - 10:20AM"
    void add_location(std::string);
    void add_instructor(std::string);
    void set_info(std::string number[]);                             // Set quota, enrol, avail, wait respectively

    // Accessors
    const Session* get_time() const {return &time[0];}  // Return a pointer to time, then we can access time[0] and time[1]
    char get_type() const {return type;}
    int get_id() const {return id;}
    int get_credits() const{return credits;}
    std::string get_course_name() const{return course_name;}
    std::string get_class_name() const{return course_name+" "+type+ std::to_string(id);}
    std::string get_code() const{return code;}
    std::string get_instructor() const{return instructor;}
    std::string get_location() const{return location;}
    std::string get_status() const{return "Enrol: "+enrol+"/"+quota;}
    bool is_empty();
    void print();
};

#endif // LECTURE_H_
