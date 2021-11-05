#include "course.h"
#include "helper.h"
#include <fstream>
#include <QDir>
#include <QCoreApplication>

using namespace std;


Course::Course(string coursename){
    // normalize the name of course
    course_name = capitalize(coursename);

    // Get current working directory in a Qt application
    string path =QCoreApplication::applicationDirPath().toStdString();
    int buildPos = path.find("build");
    string subPath = path.substr(0,buildPos);

    // open 2-line file containing all courses of the semester in the second line
    ifstream ifs(path+"/courses/available_courses.txt");
    string info;
    getline(ifs,info,'\n'); // info carry the name of semester e.g Fall 2019
    getline(ifs,info,'\n'); // info carry all course of the semester

    if(info.find(course_name) != -1){ // if exists, just play safe because in GUI we deny invalid courses!
        ifs.close();
        ifs.open(path+"/courses/"+course_name+ ".html"); // Open the corresponding file
        if(!ifs) {// check if the file exists, just play safe in case some user unconciously delete relevant data
                    cerr << "Error! Cannot find the file containing data of"<< course_name <<endl;
                }
        else{
            cout << "Opening courses/"+course_name+ ".html\n";
            getline(ifs, info,'\n');// why we use to string? Using .substr and .find
                                          // get the number of credits, "4 unit", 5 character front!
            credits = info[info.rfind('unit')-5] - '0';

            // Start up
            Class* new_class = nullptr;                        // will be used to build "lecture" and "tutorial"
            string short_key = " align=\"center\">";         // use keyword to find the position of data
            string long_key  = " align=\"center\" rowspan="; 
            const string pre = "<td>", suf = "</td>";
            size_t begin_  = 0, end_ = 0, time_sepa = 0;  // store the position of data

            // Get informations about classes
            while (!ifs.eof()){
                getline(ifs, info,'\n');                      // This may be the 1st or 3rd line of a block (line #30, lecture.h)
                begin_ = end_ = time_sepa = 0;     // reset variable
                string key;

                // If ha contains short_key or long_key, it indicates a class
                if(info.find(short_key) != -1)
                    key = short_key;
                else if(info.find(long_key) != -1){
                    // Add 4 character to the long key
                    key = info.substr(info.find(long_key),long_key.length() + 4);
                    cout << key << endl;
                }
                else if(info.find("&gt;") == 0) {      // in case the "Remark" makes a new (3rd) line, we add info to current class
                    cout << "Next line because of remark" << endl;
                    while(true){
                        begin_ = info.find(pre, end_);
                        end_ = info.find(suf, begin_+1);
                        if(begin_ == -1 or end_ == -1) break;
                        begin_ += 4;

                        if(info.substr(begin_,end_-begin_).find("0PM") != -1 or
                            info.substr(begin_,end_-begin_).find("0AM") != -1){ // Check if this segment carries time
                            time_sepa = info.find("<br>", begin_+1)+4;               // check unusual case, inspect line #108
                            if(begin_ < time_sepa and time_sepa < end_)           // time can be empty char array, we will check that
                                new_class->add_session(info.substr(time_sepa, end_ - time_sepa).c_str()); 
                            else new_class->add_session(info.substr(begin_,end_-begin_).c_str());
                        }
                    }
                    continue;    // find next class
                }
                else continue; // irreavant information

                // That line and the next will be a set of information for a class
                // First line, get class type, id, and "code to enrol"
                    begin_ =  info.find(key) + key.length();
                    const char* code_session = info.substr(begin_,info.find("</td>")-begin_).c_str(); // convert to C-string to use sscanf()
                    int id; char code[10]; begin_ = 0;

                    // Check if class is T (or R for UROP), A, or L, then take the ID, add to the corresponding vector<Course*>
                    // Courses have different type+ID, e.g Lang has T01, but Math has L1, T1C. 
                    // That's why we need to scan, and id must be 'int'
                    if(code_session[0] == 'L' and code_session[1] != 'A'){      // lecture
                        sscanf(code_session+1, "%d %s", &id, code);
                        new_class = new Class(course_name,'L',id,credits);
                        lecture.push_back(new_class);
                    }
                    else if(code_session[0] == 'L' and code_session[1] == 'A'){ // lab
                        sscanf(code_session+2,"%d %s", &id, code);
                        new_class = new Class(course_name,'A',id,credits);
                        tutorial.push_back(new_class);
                    }
                    else if(code_session[0] == 'T' or code_session[0] == 'R'){  // tutorial or UROP
                        sscanf(code_session+1,"%d %s", &id, code);
                        new_class = new Class(course_name,code_session[0],id,credits);
                        tutorial.push_back(new_class);
                    }

                // Second line gives infomation about instructor, session, ...
                    getline(ifs, info,'\n');
                    begin_ = end_ = time_sepa = 0;
                    
                    // It's time to use info.find("</td>") !
                    // take time from "<td>03-SEP-2019 - 24-SEP-2019<br>Tu 07:30PM - 09:20PM</td>"
                    //             or "<td>Mo 01:30PM - 02:50PM</td>", so we need to consider 2 cases
                    begin_ = info.find(pre)+4;
                    time_sepa = info.find("<br>", begin_+1)+4;
                    end_ = info.find(suf, begin_+1);
                    if(begin_ < time_sepa and time_sepa < end_)
                        new_class->add_session(info.substr(time_sepa, end_ - time_sepa).c_str());
                    else new_class->add_session(info.substr(begin_,end_-begin_).c_str());

                    // Location
                    begin_ = info.find(pre, end_+1)+4;
                    end_ = info.find(suf, begin_+1);
                    new_class->add_location(info.substr(begin_,end_-begin_));
                    // Why do we need to do like that, because some case the infomation is TBA, so we cannot scan directly

                    // Instructors' names
                    begin_ = info.find(pre, end_+1)+4;
                    end_ = info.find(suf, begin_+1);
                    if(info.substr(begin_,end_-begin_) == "TBA"){
                        new_class->add_instructor("TBA");
                    }
                    else{ // Some cases behave like "<td><a href="/wcq/cgi-bin/1910/instructor/NG, Shi Chung">NG, Shi Chung</a><br><a href="/wcq/cgi-bin/1910/instructor/WONG, Raymond Sze Chung">WONG, Raymond Sze Chung</a></td>", so we check to get all instuctors
                        size_t b_ = begin_, e_= begin_;
                        while(true){
                            b_ = info.find("\">", e_+1);              // sample is ">WONG, Raymond Sze Chung</a>
                            e_ = info.find("</a>", b_+1);            // = "> + WONG, Raymond Sze Chung + </a>
                            if(b_ >= end_ or b_ == -1) break;   // check if b_ is between begin_ and end_
                            b_ += 2;
                            new_class->add_instructor(info.substr(b_,e_-b_));
                        }
                    }

                    // Quota, Enrol, Avail, Wait
                    string tired[4] = {};
                    for(int i = 0; i < 4; ++i){
                        begin_ = info.find(key,end_+1)+key.length(); // allowed to plus key length because not end yet
                        end_ = info.find(suf,begin_+1);
                        tired[i] = info.substr(begin_,end_-begin_);
                        if(tired[i].find("<strong>") != -1){                 // check if style is strong (to indicate the class is full)
                            size_t temp1 = tired[i].find("<strong>")+8;
                            size_t temp2 = tired[i].find("</strong>") -temp1;
                            tired[i] = tired[i].substr(temp1,temp2);
                        }
                        if(tired[i].find("<span>") != -1){                   // check if some window embedded to the number
                            size_t temp = tired[i].find("<span>");
                            tired[i] = tired[i].substr(temp+6,tired[i].find("</span>") -temp-6);
                        }
                    }
                    new_class->set_info(tired);

                    // Find if having other session, syntax is the same as getting time above
                    while(true){
                        begin_ = info.find(pre, end_);
                        end_ = info.find(suf, begin_+1);
                        if(begin_ == -1 or end_ == -1) break;
                        begin_ += 4;

                        if((begin_ < end_) and (info.substr(begin_,end_-begin_).find("0PM") != -1 or
                            info.substr(begin_,end_-begin_).find("0AM") != -1)){
                            time_sepa = info.find("<br>", begin_+1)+4;
                            if(begin_ < time_sepa and time_sepa < end_)
                                new_class->add_session(info.substr(time_sepa, end_ - time_sepa).c_str());
                            else new_class->add_session(info.substr(begin_,end_-begin_).c_str());
                        }
                    }
            }

            // Check whether this course consists of all TBA-time classes, so we will not consider!
                // Actions: eliminate all TBA class
                for(int i =  lecture.size()-1; i  >= 0; --i){   // be careful with vector, use erase()
                    if(lecture[i]->is_empty())                    // TBA class
                        lecture.erase(lecture.begin()+i);
                }
                for(int i = tutorial.size()-1; i  >= 0; --i){
                    if(tutorial[i]->is_empty())                   // TBA class
                        tutorial.erase(tutorial.begin()+i);
                }
                // Some notifications in CLI for checking purpose
                if(is_empty()) cout << "This course consists of all TBA-time classes, we don't support and it will not overlap to any other course!" << endl;
                else cout << "We eliminate all TBA classes, since they don't overlap to any other classes!\n";

        }
    }
    else cout << "This course does not exist in the semester" << endl;
    ifs.close();
}

Course::~Course(){
   while(!lecture.empty()) {
       delete lecture.back();
       lecture.pop_back();
   }
   while(!tutorial.empty()) {
       delete tutorial.back();
       tutorial.pop_back();
   }
}

void Course::print(){
    cout << course_name << endl;
    cout << "Number of units: " << credits << endl;
    cout << "Number of lectures: " << lecture.size() << endl;
    cout << "Number of tutorials: " << tutorial.size() << endl;
    for(int i = 0; i < lecture.size(); ++i){
        lecture[i]->print();
    }
    for(int i = 0; i < tutorial.size(); ++i){
        tutorial[i]->print();
    }
    cout <<'\n';
}

bool Course::is_empty(){
    return lecture.size() <= 0 && tutorial.size() <= 0;
}

int Course::get_num_classes() const{
    int size = max(lecture.size()+tutorial.size(), tutorial.size() * lecture.size());  // count how many possible schedule of a course
    return size;
}

vector<Course*> sort_by_num_classes(vector<Course*> courselist){
    int size = courselist.size();
    for(int i = 0; i < size; ++i){
        for(int j = i+1; j < size; ++j){
            if(courselist[i]->get_num_classes() > courselist[j]->get_num_classes() ){
                Course* temp = courselist[i];
                courselist[i] = courselist[j];
                courselist[j] = temp;
            }
        }
    }
    return courselist;
}

vector<Course*> extract(vector<Course*> courselist, int bit_string){
    vector<Course*> output;
    int size = courselist.size();
    for(int i = 0; i < size; ++i)
        if((bit_string/(pow(2,i)))%2)             // each bit will determine if a course is chosen or not
            output.push_back(courselist[i]);    // Remark: when the input is sorted, the output is also sorted (credit-wise)
    return output;
}

int total_credits(vector<Course*> courselist){
    int output = 0;
    int size = courselist.size();
    for(int i = 0; i < size; ++i)
        output += courselist[i]->get_credits();
    return output;
}
