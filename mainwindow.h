/* This class is used to create main window that user can interact with.
 * Object impl will receive all data from user: list of courses that users want to enroll, time constraints, when user click button update_data or proceed
 * The class also have many slots that can be devided into 2 groups:
 * 1. Slots when user interacts with table, button or editline
 * 2. Slots generate results for users when executive functions is finished 
 *
 * Function setBackgroundListCourses(): when user double enter course that already has been in list courses they want to enroll, the background color of
 *                                      this course with change to gray color to inform user. this function used to change back to white color when there 
 *                                      is any interaction from user rightafter that
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QObject>
#include <QStringList>
#include "impl.h"
#include <QtWidgets>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <QPixmap>
#include <QCompleter>
#include <QTimer>
using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setBackgroundListCourses(); // Change background of listCourses to white color


private slots:
    //slots when users interact with table, button or editline
    void addCourseSlot(const QString &text);              // When user clicks on name of a course, this course will be added to the course of list user wants to add
    void on_TableWidget_cellDoubleClicked(int row, int column);
            // For user to inform that he/she will be busy at any cell on timetable. User can double click again to remove this time constraint
            // why we do not use toggle? because we need to choose the cell to input reason for constraining, so double click to switch status is suitable
    void on_listCourses_itemDoubleClicked(QListWidgetItem *item);   // Remove this course in the list course that user wants to enrol
    void on_proceedButton_clicked();                        // To start the process of finding suitable timetables
    void on_updateButton_clicked();                          // Open a dialog for user to add the link of semester (database) that they want to update data
    void on_exportButton_clicked();                          // To export the current time table into an image file
    void on_removeAllButton_clicked();                   // Remove all courses in the course list that user wanted to enrol

    //slots when executive functions finish, and it send information back to user
    void createNewTimeTable(Timetable*tempTimetable);    // Create new timetable in a new tab with the information of tempTimetable
    void noScheduleDialog();                                               // Create a dialog to inform that there is no suitable timetable for all courses
    void maxCreditsScheduleDialog();                                // Create a dialog to inform user that a timetable with max credits is provided
    void finishUpdateDialog(vector<string>);                        // Inform user that data has been updated
    void updateData();


private:
    Ui::MainWindow *ui;
    Impl* impl;
    QCompleter *completer; // This data member help user to find and choose course want to add

};
#endif // MAINWINDOW_H
