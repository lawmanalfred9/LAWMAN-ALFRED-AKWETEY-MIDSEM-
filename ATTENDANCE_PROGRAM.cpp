#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <ctime>
#include <sstream>
#include <cstddef> 
#include <cctype>  

using namespace std;

// --- Student Class ---
class Student {
private:
    string indexNumber;
    string name;

public:
    Student() {} 
    Student(string idx, string n) : indexNumber(idx), name(n) {}

    string getIndex() const { return indexNumber; }
    string getName() const { return name; }

    void saveToFile(ofstream& outFile) const {
        // Excel-friendly CSV format
        outFile << indexNumber << "," << name << endl;
    }
    
    void displayFormatted() const {
        cout << left << setw(15) << indexNumber << setw(30) << name << endl;
    }
};

// --- Attendance Record ---
struct AttendanceRecord {
    string studentIndex;
    string status;
};

// --- Session Class ---
class AttendanceSession {
private:
    string courseCode;
    string date;
    string startTime;
    string duration;
    vector<AttendanceRecord> records;

public:
    AttendanceSession(string code, string dt, string st, string dur)
        : courseCode(code), date(dt), startTime(st), duration(dur) {}

    void addRecord(string index, string status) {
        AttendanceRecord newRecord;
        newRecord.studentIndex = index;
        newRecord.status = status;
        records.push_back(newRecord);
    }

    string generateFilename() const {
        return "session_" + courseCode + "_" + date + ".csv";
    }

    void saveToFile() const {
        string filename = generateFilename();
        ofstream outFile(filename.c_str()); 

        if (outFile.is_open()) {
            // Write headers for Excel columns
            outFile << "Course Code,Date,Start Time,Duration" << endl;
            outFile << courseCode << "," << date << "," << startTime << "," << duration << endl;
            outFile << endl << "Student Index,Status" << endl;
            
            for (size_t i = 0; i < records.size(); ++i) {
                outFile << records[i].studentIndex << "," << records[i].status << endl;
            }
            outFile.close();
            cout << "\nSuccess! Excel file created: " << filename << endl;
        } else {
            cerr << "Error: Could not create Excel file." << endl;
        }
    }
};

// --- Helper functions ---
string getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "_"
       << setfill('0') << setw(2) << 1 + ltm->tm_mon << "_"
       << setw(2) << ltm->tm_mday;
    return ss.str();
}

void loadStudentsFromFile(vector<Student>& students) {
    ifstream inFile("students.csv"); 
    if (!inFile) return; // File doesn't exist yet, that's okay

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string index, name;
        if (getline(ss, index, ',') && getline(ss, name)) {
            students.push_back(Student(index, name));
        }
    }
    inFile.close();
}

void registerStudent(vector<Student>& students) {
    string index, name;
    cout << "Enter student index: "; cin >> index;
    cout << "Enter student name: "; 
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, name);

    Student s(index, name);
    students.push_back(s);

    ofstream outFile("students.csv", ios::app); 
    if (outFile.is_open()) {
        s.saveToFile(outFile);
        outFile.close();
        cout << "Student saved to students.csv\n";
    }
}

void markAttendance(vector<Student>& students) {
    if (students.empty()) {
        cout << "Error: No students registered.\n";
        return;
    }

    string code, start, dur;
    string date = getCurrentDate();
    cout << "Course Code: "; cin >> code;
    cout << "Start Time (HH:MM): "; cin >> start;
    cout << "Duration: "; cin >> dur;

    AttendanceSession session(code, date, start, dur);
    for (size_t i = 0; i < students.size(); ++i) {
        char status;
        do {
            cout << "Status for " << students[i].getName() << " [P/A/L]: ";
            cin >> status;
            status = toupper(status);
        } while (status != 'P' && status != 'A' && status != 'L');
        
        string s(1, status);
        session.addRecord(students[i].getIndex(), s);
    }
    session.saveToFile();
}

void displayReports() {
    string code, date;
    cout << "Course Code: "; cin >> code;
    cout << "Date (YYYY_MM_DD): "; cin >> date;

    string filename = "session_" + code + "_" + date + ".csv";
    ifstream inFile(filename.c_str());

    if (!inFile.is_open()) {
        cout << "Report file not found.\n";
        return;
    }

    string line;
    // Skip Excel headers to get to the data
    for(int i = 0; i < 4; ++i) getline(inFile, line); 

    cout << "\n--- Attendance Records ---\n";
    string idx, status;
    while (getline(inFile, line)) {
        stringstream ss(line);
        if (getline(ss, idx, ',') && getline(ss, status)) {
            cout << "Index: " << left << setw(15) << idx << " Status: " << status << endl;
        }
    }
    inFile.close();
}

int main() {
    vector<Student> students;
    loadStudentsFromFile(students);

    int choice = 0;
    while (choice != 5) {
        cout << "\n1. Register Student\n2. View Students\n3. Mark Attendance (Excel)\n4. View Report\n5. Exit\nChoice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1) registerStudent(students);
        else if (choice == 2) {
            for (size_t i = 0; i < students.size(); ++i) students[i].displayFormatted();
        }
        else if (choice == 3) markAttendance(students);
        else if (choice == 4) displayReports();
    }
    return 0;
}