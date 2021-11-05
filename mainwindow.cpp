#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Course Arranger");
    ui->tabWidget->setTabText(0, "Time constraints");
    impl = new Impl();
   QObject::connect(impl,SIGNAL(callCreateNewTable(Timetable*)),this,SLOT(createNewTimeTable(Timetable*)));
   QObject::connect(impl,SIGNAL(noSchedule()),this,SLOT(noScheduleDialog()));
   QObject::connect(impl,SIGNAL(maxCreditsSchedule()),this,SLOT(maxCreditsScheduleDialog()));
   QObject::connect(impl,SIGNAL(finishUpdate(vector<string>)),this,SLOT(finishUpdateDialog(vector<string>)));


   //get all courses available in the semester
   //Get current working directory in a Qt application
   string path =QCoreApplication::applicationDirPath().toStdString();
   cout << path << endl;
   int buildPos = path.find("build");
   string subPath = path.substr(0,buildPos);
   ifstream ifs(path+"/courses/available_courses.txt");
   string semester; string coursestring;
   getline(ifs, semester,'\n');
   ui->currentTerm->setText(semester.c_str());
   getline(ifs, coursestring, '\n');
   unsigned int size = coursestring.length()/11;
   QStringList courses;
   for(int i = 0; i < size; ++i){
        string temp = coursestring.substr(11*i,10);
        //cout << temp << endl;
        if(temp[9] != ' ') courses << temp.c_str();
        else  courses << temp.substr(0,9).c_str();
   }
   ifs.close();
    //search and add courses
   completer = new QCompleter(courses,ui->addCourse);
   completer->setCaseSensitivity(Qt::CaseInsensitive);
   ui->addCourse->setCompleter(completer);
   QObject::connect(completer,SIGNAL(activated(QString)),this,SLOT(addCourseSlot(QString)),Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::addCourseSlot(const QString &text)
{
    setBackgroundListCourses();

    bool existed =false;
    int i=0;
    for(;i<ui->listCourses->count();++i)
        if(ui->listCourses->item(i)->data(Qt::DisplayRole)==text){
            existed=true;break;}
    if(!existed)
        ui->listCourses->addItem(text);
    else
        ui->listCourses->item(i)->setBackgroundColor("gray");
    ui->addCourse->clear();
}
void MainWindow::on_listCourses_itemDoubleClicked(QListWidgetItem *item)
{
    // delete clicked course
    if(!ui->checkBox->isChecked()){
        setBackgroundListCourses();
        item->~QListWidgetItem();
    }
    else{
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Remove course", "Do you want to remove this course?",
                                        QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
            setBackgroundListCourses();
            item->~QListWidgetItem();
            }
    }
}

void MainWindow::on_removeAllButton_clicked()
{
    //delete all courses
    if(!ui->checkBox->isChecked())
        ui->listCourses->clear();
    else{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Remove all courses", "Do you want to remove all courses?",
                                    QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
        setBackgroundListCourses();
        ui->listCourses->clear();
        }
    }

}

void MainWindow::setBackgroundListCourses(){
    int num = ui->listCourses->count();
    for(int i=0;i<num;++i)
        ui->listCourses->item(i)->setBackgroundColor("white");
}
void MainWindow::on_TableWidget_cellDoubleClicked(int row, int column)
{
    if(!ui->checkBox->isChecked()){
        if(!impl->isTimeConstrains(row,column)){
            setBackgroundListCourses();
            //ui->TableWidget->setStyleSheet("QTableWidget {selection-background-color: gray;}");
            ui->TableWidget->setItem(row, column, new QTableWidgetItem(""));
            ui->TableWidget->item(row,column)->setBackgroundColor("gray");
            impl->addTimeConstrains(row,column);
        }
        else{
            setBackgroundListCourses();
            ui->TableWidget->setItem(row, column, new QTableWidgetItem(""));
            ui->TableWidget->item(row,column)->setBackgroundColor("white");
            impl->removeTimeConstrains(row,column);
        }
    }
    else{
          if(!impl->isTimeConstrains(row,column)){
              setBackgroundListCourses();
              //ui->TableWidget->setStyleSheet("QTableWidget {selection-background-color: gray;}");
              ui->TableWidget->setItem(row, column, new QTableWidgetItem(""));
              ui->TableWidget->item(row,column)->setBackgroundColor("gray");
              impl->addTimeConstrains(row,column);
          }
          else{
              QMessageBox::StandardButton reply = QMessageBox::question(this, "Remove time constrains", "Do you want to remove this time constrain?",
                                              QMessageBox::Yes|QMessageBox::No);
               if (reply == QMessageBox::Yes) {
                  setBackgroundListCourses();
                  ui->TableWidget->setItem(row, column, new QTableWidgetItem(""));
                  ui->TableWidget->item(row,column)->setBackgroundColor("white");
                  impl->removeTimeConstrains(row,column);
               }
          }
    }
}

void MainWindow::on_proceedButton_clicked()
{
    setBackgroundListCourses();
    //remove all current timetables
    while(ui->tabWidget->count()>1)
        ui->tabWidget->removeTab(1);
    //remove all courses on course list in timetable imformation part
    ui->listCourses_2->clear();
    ui->totalCredit->setNum(0);
    // add list of courses
    string* arrayCourses= new string[ui->listCourses->count()];
    for(int i=0;i<ui->listCourses->count();++i)
        arrayCourses[i] = ui->listCourses->item(i)->data(Qt::DisplayRole).toString().toStdString();
    impl->addCourse(arrayCourses,ui->listCourses->count());
    impl->generate();
}
void MainWindow::createNewTimeTable(Timetable*tempTimetable){
    //set color for timetable
    vector<QString> color = {"PeachPuff", "MintCream", "SlateBlue",
                             "Khaki", "OliveDrab","BurlyWood", "LightSalmon", "Coral",
                             "HotPink", "LemonChiffon", "AliceBlue", "LightGrey",
                             "MediumTurquoise", "DarkSeaGreen", "Peru", "Tomato",
                             "Orchid","PapayaWhip"};
    srand((unsigned)time(0));
    int colorIndex = (rand()%11)+1;

    //set font type for timetables
    QFont fnt1;
    fnt1.setPointSize(8);
    fnt1.setFamily("Times New Roman");

    //create new table for each timetable
    QTableWidget* timetable = new QTableWidget;
    timetable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tabWidget->addTab(timetable,QString("Tab %0").arg(ui->tabWidget->count()));
    ui->tabWidget->setCurrentIndex(1);
    timetable->setRowCount(28);
    timetable->setColumnCount(6);

    QStringList day;
    day<<"Mon"<<"Tue"<<"Wed"<<"Thu"<<"Fri"<<"Sat";
    QStringList session;
    session<<"9:00-9:30"<<"9:30-10:00"<<"10:00-10:30"<<"10:30-11:00"<<"11:00-11:30"<<"11:30-12:00"<<"12:00-12:30"<<"12:30-13:00"<<"13:00-13:30"<<"13:30-14:00"<<"14:00-14:30"<<"14:30-15:00"<<"15:00-15:30"<<"15:30-16:00"<<"16:00-16:30"<<"16:30-17:00"<<"17:00-17:30"<<"17:30-18:00"<<"18:00-18:30"<<"18:30-19:00"<<"19:00-19:30"<<"19:30-20:00"<<"20:00-20:30"<<"20:30-21:00"<<"21:00-21:30"<<"21:30-22:00"<<"22:00-22:30"<<"22:30-23:00";

    timetable->setHorizontalHeaderLabels(day);
    timetable->setVerticalHeaderLabels(session);

    vector<string> courses;
    int totalCredits=0;

    //add content for all cells on timetable
    for (int i=0;i<6;++i){
        string preClass="";
        for(int j=0;j<28;++j){
            const Class* classPtr=tempTimetable->table[i][j];

            if(classPtr!=nullptr){
            int index=0;
            //class information
            string className=classPtr->get_class_name();
            string courseName = classPtr->get_course_name();
            int credit = classPtr->get_credits();
            QString inforLong = QString::fromStdString("Instructor:\n"+classPtr->get_instructor()+"\n\n"+classPtr->get_status());
            QString inforShort =QString::fromStdString(classPtr->get_status());
            //create vector of all courses on timetable
            if(courses.size()==0){
                courses.push_back(courseName);
                totalCredits+=credit;
            }
            else{
                    while(index<courses.size()&&courseName!=courses[index])
                        ++index;
                    if(index==courses.size()){
                        courses.push_back(courseName);
                        totalCredits+=credit;
                    }

            }
            // group cells that are in same class

                if(className==preClass&&j>0){
                    timetable->setItem(j,i,new QTableWidgetItem(inforShort));
                    int num=0;
                    for(int k=j;k<28;++k){
                        if(tempTimetable->table[i][k]==nullptr||className!=tempTimetable->table[i][k]->get_class_name())
                            break;
                        else ++num;
                    }
                    timetable->setSpan(j,i,num,1);
                    if(courseName=="Occupied")
                        timetable->setItem(j,i,new QTableWidgetItem(""));
                    else if(num>=2)
                        timetable->setItem(j,i,new QTableWidgetItem(inforLong));
                    timetable->item(j,i)->setFont(fnt1);


                }
                else{
                    if(courseName=="Occupied")
                        timetable->setItem(j,i,new QTableWidgetItem("Occupied"));
                    else{
                    QString qstr = QString::fromStdString(className);
                    timetable->setItem(j,i,new QTableWidgetItem(qstr));
                    }
                    preClass=className;


                }
            //set background color corresponding to each course
            timetable->item(j,i)->setBackgroundColor(color[index+colorIndex]);
        }
        }
    }
    //present list of courses and total credits of timetable:
    ui->listCourses_2->clear();
    for(int i=0;i<courses.size();++i){
        if(courses[i]!="Occupied")
            ui->listCourses_2->addItem(QString::fromStdString(courses[i]));
    }
    ui->totalCredit->setNum(totalCredits);
}
void MainWindow::noScheduleDialog(){
    ui->totalCredit->setNum(0);
    QMessageBox msgBox;
    msgBox.setText("There is no suitable schedule!");
    msgBox.exec();
}
void MainWindow::maxCreditsScheduleDialog(){
    QMessageBox msgBox;
    msgBox.setText("There is no suitable schedule!\nWe recommend the timetable with largest number of courses");
    msgBox.exec();
}

void MainWindow::updateData(){
    bool ok; QMessageBox msgBox;
    QString text = QInputDialog::getText(this, tr("Update data"),
            tr("Enter the link to HKUST Class Schedule Quota corresponding to the semester you want to enroll\nE.g: https://w5.ab.ust.hk/wcq/cgi-bin/1910/"),
            QLineEdit::Normal, QDir::home().dirName(), &ok);
   if (ok && !text.isEmpty() && text.indexOf("https://w5.ab.ust.hk/wcq/cgi-bin/") == 0){
           string semester = text.toStdString();
           QMessageBox::StandardButton reply = QMessageBox::question(this, "Update data", "This might take several minutes. Please wait until it finishes",
                                    QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                string ses = impl->update_data(semester);
                if(ses.length() < 5) {
                    msgBox.setText("Please supply the valid link");
                    msgBox.exec();
                }
                else{
                    msgBox.setText("The data has been updated");
                    msgBox.exec();
                    ui->currentTerm->setText(ses.c_str());
                }
            }
   }

}
void MainWindow::on_updateButton_clicked()
{
    ui->listCourses->clear();
    ui->listCourses_2->clear();
    updateData();
}
void MainWindow::finishUpdateDialog(vector<string> courses){
    QStringList qcourses;
    for(int i=0;i<courses.size();++i){
        qcourses.append(QString::fromStdString(courses[i]));
    }
    //change content of search engine
    QCompleter *completer = new QCompleter(qcourses,ui->addCourse);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->addCourse->setCompleter(completer);
    QObject::connect(completer,SIGNAL(activated(QString)),this,SLOT(addCourseSlot(QString)),Qt::QueuedConnection);

    /*QMessageBox msgBox;
    msgBox.setText("The data has been updated");
    msgBox.exec();*/
}
// export current timetable into image, the code is from Screenshot::saveScreenshot() of https://doc.qt.io/qt-5/qtwidgets-desktop-screenshot-example.html
void MainWindow::on_exportButton_clicked()
{
    QPixmap originalPixmap = QPixmap::grabWidget(ui->tabWidget->currentWidget());
    const QString format = "png";
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (initialPath.isEmpty())
        initialPath = QDir::currentPath();
    initialPath += tr("/untitled.") + format;

    QFileDialog fileDialog(ui->tabWidget->currentWidget(), tr("Save As"), initialPath);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDirectory(initialPath);
    QStringList mimeTypes;
    const QList<QByteArray> baMimeTypes = QImageWriter::supportedMimeTypes();
    for (const QByteArray &bf : baMimeTypes)
        mimeTypes.append(QLatin1String(bf));
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.selectMimeTypeFilter("image/" + format);
    fileDialog.setDefaultSuffix(format);
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fileName = fileDialog.selectedFiles().first();
    if (!originalPixmap.save(fileName)) {
        QMessageBox::warning(this, tr("Save Error"), tr("The image could not be saved to \"%1\".").arg(QDir::toNativeSeparators(fileName)));
    }

}

