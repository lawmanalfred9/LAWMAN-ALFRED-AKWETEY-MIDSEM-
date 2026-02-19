#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <ctime>
#include <sstream>
#include <cstddef> // Required for size_t in C++98
#include <cctype>  // Required for toupper()

using namespace std;

// --- Student Class Definition (Functional Requirement 5.1) ---
class Student {
private:
    string indexNumber;
    string name;

public:
    Student() {} // C++98 default constructor

    Student(string idx, string n) : indexNumber(idx), name(n) {}

    string getIndex() const { return indexNumber; }
    string getName() const { return name; }

    void saveToFile(ofstream& outFile) const {
        outFile << indexNumber << " " << name << endl;
    }
    
    // Helper function for formatted display
    void displayFormatted() const {
        cout << left << setw(15) << indexNumber << setw(30) << name << endl;
    }
};

// --- Attendance Record Struct ---
struct AttendanceRecord {
    string studentIndex;
    string status;
};

// --- AttendanceSession Class Definition (Functional Requirement 5.2) ---
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
        return "session_" + courseCode + "_" + date + ".txt";
    }

    void saveToFile() const {
        ofstream outFile(generateFilename().c_str()); 

        if (outFile.is_open()) {
            outFile << "Course: " << courseCode << ", Date: " << date
                    << ", Start Time: " << startTime << ", Duration: " << duration << endl;
            for (size_t i = 0; i < records.size(); ++i) {
                outFile << records[i].studentIndex << " " << records[i].status << endl;
            }
            outFile.close();
            cout << "Session data saved to " << generateFilename() << endl;
        } else {
            cerr << "Error creating session file." << endl;
        }
    }
};

// --- Function Prototypes (Declarations) ---
// These tell the main function about the existence and structure of the functions below
void loadStudentsFromFile(vector<Student>& students);
void registerStudent(vector<Student>& students);
void markAttendance(vector<Student>& students);
void displayReports();
string getCurrentDate();

// --- Main Function (Menu-Driven Program Flow) ---
int main() {
    vector<Student> students;
    loadStudentsFromFile(students); // Now the compiler knows what this function is

    int choice = 0;
    do {
        cout << "\n==================================================" << endl;
        cout << "  Digital Attendance System Menu" << endl;
        cout << "==================================================" << endl;
        cout << "1. Register a new student" << endl;
        cout << "2. View all registered students" << endl;
        cout << "3. Create/Mark a lecture session" << endl;
        cout << "4. View Reports and Summaries" << endl;
        cout << "5. Exit" << endl;
        cout << "--------------------------------------------------" << endl;
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                registerStudent(students);
                break;
            case 2:
                cout << "\n--- Registered Students ---\n";
                cout << left << setw(15) << "INDEX NUMBER" << setw(30) << "STUDENT NAME" << endl;
                cout << "----------------------------------------------" << endl;
                for (size_t i = 0; i < students.size(); ++i) {
                   students[i].displayFormatted();
                }
                break;
            case 3:
                markAttendance(students);
                break;
            case 4:
                displayReports();
                break;
            case 5:
                cout << "Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 5);

    return 0;
}

// --- Global Functions Implementations (FR 5.4 & Helpers) ---
// The actual code for the functions defined by the prototypes above

void loadStudentsFromFile(vector<Student>& students) {
    ifstream inFile("students.txt"); 
    string index, name;
    while (inFile >> index && getline(inFile, name)) {
        size_t firstChar = name.find_first_not_of(" \\t");
        if (firstChar != string::npos) {
            name = name.substr(firstChar);
        }
        Student tempStudent(index, name);
        students.push_back(tempStudent);
    }
    inFile.close();
}

void registerStudent(vector<Student>& students) {
    string index, name;
    cout << "Enter student index number: ";
    cin >> index;
    cout << "Enter student name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, name);

    Student tempStudent(index, name);
    students.push_back(tempStudent);

    ofstream outFile("students.txt", ios::app); 
    if (outFile.is_open()) {
        students.back().saveToFile(outFile);
        outFile.close();
        cout << "\nStudent registered and saved successfully." << endl;
    } else {
        cerr << "Error opening file for writing." << endl;
    }
}

string getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "_"
       << setfill('0') << setw(2) << 1 + ltm->tm_mon << "_"
       << setw(2) << ltm->tm_mday;
    return ss.str();
}

void markAttendance(vector<Student>& students) {
    if (students.empty()) {
        cout << "No students registered yet." << endl;
        return;
    }

    string code, start, duration;
    string date = getCurrentDate();
    cout << "Enter Course Code (e.g., EEE227): ";
    cin >> code;
    cout << "Enter Start Time (HH:MM): ";
    cin >> start;
    cout << "Enter Duration (hours): ";
    cin >> duration;

    AttendanceSession session(code, date, start, duration);
    cout << "\n--- Marking attendance for " << code << " on " << date << " ---\n";

    for (size_t i = 0; i < students.size(); ++i) {
        char status_char;
        do {
            cout << "Status for " << students[i].getName() << " (" << students[i].getIndex() << ") [P/A/L]: ";
            cin >> status_char;
            status_char = toupper(status_char); 
        } while (status_char != 'P' && status_char != 'A' && status_char != 'L');
        
        string status_str(1, status_char);
        session.addRecord(students[i].getIndex(), status_str);
    }
    session.saveToFile();
}

void displayReports() {
    cout << "\n--- View Reports and Summaries ---\n";
    cout << "This functionality needs to be implemented to read session files and display formatted summaries." << endl;
}
