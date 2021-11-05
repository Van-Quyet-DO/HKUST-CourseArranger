/* This is the main part of our porject
 * The list of courses that users want to enroll in will be added to courselist in the function addCourse()
 * For time constraints, when user interact with the "Time Constraints" table, time constraints will be added or removed according to the actions of user
 *
 * When user press "Proceed" Button, the function generate() will be called. This generate() function use data member courselist to find all possible timetables
 * Function update_data(string semester): the string semester is consider the link of HKUST Class Schedule & Quota corresponding to the semester
 * that user wants to update. After call update_data(), all the data in folder courses and data are changed according to the information of this semester
 *
 * This class also has some signals so that after execute function generate(), the information of possible timetable will be presented on screen
 */

#ifndef MAINIMPLEMENT_H
#define MAINIMPLEMENT_H
#include "course.h"
#include "timetable.h"
#include "urlmon.h"
using namespace std;
#pragma comment (lib, "urlmon.lib")
#include <fstream>
#include <QCoreApplication>
#include <QObject>

class Impl:public QObject
{
Q_OBJECT
public:
    Impl();

    void addCourse(string* course, int len);                  // Add the parameter course of given len to the data member courselist
    void addTimeConstrains(int row,int colummn);      // Add time constraints to the cell coordinated (row, col) of the base table
    void removeTimeConstrains(int row, int column); // Remove time constraints of the cell coordinated (row, col) from the base table
    bool isTimeConstrains(int row, int column);           // Check whether there is any time constraint in the cell coordinated (row, col)
    void generate();                                                    // Generate all the possible timetables if there is any; otherwise, generate the one with maximum credits
    string update_data(string semester_url);                 // Update all the necessary information (time sessions, quotas...) from the database
                                                                                 // and return the name of corresponding semester

private slots:
    void implCreateNewTable(Timetable*);
signals:
    void callCreateNewTable(Timetable*);    // When we find a possible timetable, this signal is called to present the timetable on screen
    void noSchedule();                                  // When we cannot find any timetable that is suitable for all courses, this signal is emited to inform user
    void maxCreditsSchedule();                   // If we cannot find any timetable, we will provide the timetable with max credits, so with this signal
                                                                   // user will know that a schedule with max credits is provided
    void finishUpdate(vector<string>);           // When finish update, the signal is emited

private:
    vector<Course*> courselist;       // All courses that users want to add
    Course* adder=nullptr;             // With the name of course, we will create an course object called adder. The conventional constructor in course class
                                                    // will be called to construct all data for this course object
    Timetable base;                         // We consider base is our main timetable to check can or cannot add a class


};

#endif // MAINIMPLEMENT_H
