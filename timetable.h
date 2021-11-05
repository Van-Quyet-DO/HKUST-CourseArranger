/* "Timetable" objects simulate a real timetable with timeslots,
 * be used to arrange courses to prevent time crashing and store valid schedule to display in GUI
 */

#ifndef TIMETABLE_H_
#define TIMETABLE_H_

#include "course.h"
#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QObject>
using namespace std;

class Timetable: public QObject{
        Q_OBJECT
    friend class MainWindow;

    static const int MAX_NUMBER_OF_TABLES = 50;
    static int count_table;          // Prevent export to much timetables
    int num_credits;
    const Class* table[6][28];  /* no. days per week x no. time slots per day. The reason why we use Class* instead of string is that
                                             * We want to show some information such as name_course, type, section, etc. as in HKUST Timetable Planner.
                                             * Each timecell either is empty or belongs to a class (including time_constraint class),
                                             * then we can take suitable info to display.
                                             * We consider each cell of time_contraint as a class which will help all cells timetable in the same class type.
                                             * Therefore, the way to find a possible timetable and create a timetable in file mainwindow.cpp will be easier
                                             */
    bool add_class(const Class*, string course_name); // When add a course to table, we need to add class! 
    void remove_class(string title);                             // Sometimes we only want to remove a class but not all the course

public:
    // Constructor and Destructor
    Timetable(); 
    ~Timetable();

    // Core Mutators, as the core of whole program
    bool fit(Course** input, int remain, bool need_print = true, bool take_one = false); 
        /* check if it has a valid schedule consisting of a list of courses. There are two options of output, controlled by 
         * need_print: "false" if we only want to check whether the list has a valid schedule (used in max_credit())
         * take_one: "yes" if we only want to print out one schedule among those possible as the suggestion or verification
         * Other variable
         * "input" is Course** instead of vector<Course*> to make the recursive call easier
         * "remain" avoids us access memory beyond the courselist (vector<Course*>)
         * The idea is using RECURSION
         */
    bool max_credits(vector<Course*>); // determine the schedule with the largest number of credits

    // Normal Mutators
    bool add_course(const Course*);      // As the primary implementation for fit()
    void add_time_constraint(int d, int s);  
    void remove_time_constraint(int d, int s);
    void clean_keep_constraint();
    void clean_all();
    void remove_course(const Course*);
    void reset_counter(){count_table = 0;}

    // Accessors
    bool can_export() const {return count_table < MAX_NUMBER_OF_TABLES;}  // if ID is too large, i.e we have too much table, we only export ~ 50 tables
    bool is_time_constraint(int d,int s);
    Timetable* print();   /* This function will create a new timetable, and copy all data from the possible timetable
                                    * In the function fit, we only use a timetable for whole process to check a class or course can be added or not
                                    * so each time it finds a possible timetable, creating a new timetable is needed
                                    * and the new timetable will be sent through signals newTimeTable(Timetable*) to present on screen
                                    */

signals:                                            // after finding a possible timetable, or the finding timetable with largest of number of courses
    void newTimeTable(Timetable*); // the signal will be created and send this timetable to impl and mainwindow to present on screen
};

#endif // TIMETABLE_H_
