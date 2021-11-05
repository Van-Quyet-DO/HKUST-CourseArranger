#include "impl.h"

Impl::Impl(){
    QObject::connect(&base,SIGNAL(newTimeTable(Timetable*)),this,SLOT(implCreateNewTable(Timetable*)));
}

void Impl::addCourse(string* course, int len){
    courselist.clear();
    for (int i=0;i<len;++i){
        adder = new Course(course[i]);
        if(!adder->is_empty())
            courselist.push_back(adder);
        else delete adder;
    }
    courselist=sort_by_num_classes(courselist);

}
void Impl::implCreateNewTable(Timetable* tempTimetable){
    emit callCreateNewTable(tempTimetable);
}

/* There is a conflict here because the way we construct table in file mainwindow.cpp: session*day and in file timetable day*session
 * we wrote all files to execute first then find the way to demonstrate in UI
 * so it is better to small change the table in this step to adapt with all execute files that we had written
 */
void Impl::addTimeConstrains(int row, int column){
    base.add_time_constraint(column,row);
}
void Impl::removeTimeConstrains(int row, int column){
    base.remove_time_constraint(column,row);
}
bool Impl::isTimeConstrains(int row, int column){
    return base.is_time_constraint(column,row);
}

void Impl::generate(){
    int size = courselist.size();
    base.reset_counter();
    // Try to print all possible schedules. Keep 'base' as the source of time constraints
    if(!base.fit(&courselist[0], courselist.size(),true, false)){ // check if all courses can be included
        cout << "\t\t\tThere is no suitable schedule!\n\n\n" << endl;
        // if no suitable schedule, print a timetable with largest number of credits
        if(base.max_credits(courselist))
            emit maxCreditsSchedule();
        else
            emit noSchedule();
    }

}
string Impl::update_data(string semester = "https://w5.ab.ust.hk/wcq/cgi-bin/1910/"){
    // the input should be https://w5.ab.ust.hk/wcq/cgi-bin/1910/..., as the link to HKUST Class and Quota website
    // take SCIE because all semesters have this course
    // step 1: download SCIE, open SCIE file
    // step 2: find list of all the category '<div class="depts">', list of all courses 'var allcourses =', print courses exist in this semester to avoid invalid courses
    // step 3: download big files
    // step 4: split to course files

    // Step 1:
    string model = "https://w5.ab.ust.hk/wcq/cgi-bin/1910/";
    size_t length = model.length();
    model = semester.substr(0,length);
        // Analize link to find which semester the data of
        string ses = "";
        switch(model[length-3]-'0'){
            case 1:	ses = "Fall "; break;
            case 2:	ses = "Winter "; break;
            case 3:	ses = "Spring "; break;
            case 4:	ses = "Summer "; break;
            default: return "";
        }
        ses += "20" + model.substr(length-5,2); // name of semester

    string url = model + "subject/SCIE";
    LPCSTR lpcURL = url.c_str();

    //Get current working directory in a Qt application
    string path =QCoreApplication::applicationDirPath().toStdString();
    int buildPos = path.find("build");
    string subPath = path.substr(0,buildPos);
    string destination = path+"/data/SCIE.html";
    LPCSTR lpcDestination = destination.c_str();

    // Call function to download content
    HRESULT hr = URLDownloadToFileA( nullptr, lpcURL, lpcDestination, 0, nullptr );
    ifstream ifs(path+"/data/SCIE.html");
    ofstream ofs(path+"/courses/available_courses.txt");
    ofs << ses << "\n"; // write the name of semester to a data file

    // Step 2:
    string key_category = "<div class=\"depts\">";
    string key_courses = "var allcourses = ['";
    vector<string> category(0);
    vector<string> courses(0);
    string info;

        // Take info in SCIE file
        while(!ifs.eof()){
            getline(ifs,info, '\n');
            if(info.find(key_category) == 0){ // found at the begin of the line
                // find </a> then take 4 characters before that
                size_t pos = info.find("</a>");
                while(pos != -1){
                    cout << info.substr(pos-4,4) << endl;
                    category.push_back(info.substr(pos-4,4));
                    pos = info.find("</a>", pos+1);
                }
            }
            else if(info.find(key_courses) == 0) {  // found at the begin of the line
                size_t pos = key_courses.length()-3;
                while(pos != -1){
                    string temp = info.substr(pos+3,10); ofs << temp << ',';
                    cout << info.substr(pos+3,10) << endl;
                        if(temp[9] != ' ')	courses.push_back(temp);
                        else courses.push_back(temp.substr(0,9));
                    pos = info.find("','", pos+1); // key is ','
                }
                break;
            }
        }
        // double-check whether the link is valid
        if(category.size() == 0 || courses.size() == 0) return "";
        ifs.close(); ofs.close();

    // Step 3: Download big files
    int size_ = category.size(); cout << size_ << endl;
    for(int i = 0; i < size_; ++i){
        string temp = category[i];
        cout << temp << endl;

        //Convert the string to a LPCSTR type so we can use it in the URLDownloadToFile() function
        url = model + "subject/"+temp;
        lpcURL = url.c_str();
        destination =path+"/data/"+temp+".html";
        lpcDestination = destination.c_str();

        // Call function to download content
        HRESULT hr = URLDownloadToFileA( nullptr, lpcURL, lpcDestination, 0, nullptr );
    }

    // Step 4: Split to courses
    size_ = courses.size();
    string current_track = category[0];
    ifs.open(path+"/data/"+current_track+".html"); // open the big html file containing current course
    for(int i = 0; i < size_; ++i){
        string current_course = courses[i];
        if(current_course.substr(0,4) != current_track){ // check whether we need to open a new big html file
            ifs.close();
            current_track = current_course.substr(0,4);    // if not, open the new file
            ifs.open(path+"/data/"+current_track+ ".html");
            if(!ifs){
                    cerr << "Error! Can't not open this file. Please update all informations!" << endl;
                    return "";
            }
            else cout << "Opening file " + current_track + ".html"<< endl;
        }

        cout << current_course << endl; bool found =false;
        while(!found and !ifs.eof()){
            getline(ifs,info, '\n');
            cout << info.substr(0,4) << endl;
            if(info.substr(0,4) == "<h2>" and info.find(current_course) != -1){ // find the beginning of code::block containing all data of current course
                found = true;                     // if found, start to take data
                cout << "Successfully found "+current_course << endl;
                ofs.open(path+"/courses/"+current_course+".html");
                while (!ifs.eof()){
                    ofs << info << '\n';
                    getline(ifs,info, '\n');
                    if(info == "</div>") {
                        ofs.close();
                        break;                      // End of course, stop taking data
                    }
                }
            }
        }
        if(!found) cout << current_course + " does not exist at this semester" << endl;
    }


    ifs.close();
    emit finishUpdate(courses); // push the dialog notify the successful update
    return ses;                         // return the name of semester
}


