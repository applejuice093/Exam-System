#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <map>
#include <limits>

using namespace std;

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnterToContinue()
{
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

// Reads an integer, re-prompting on bad input. Exits if stdin is closed,
// so a failed read can never leave a menu looping forever.
int readInt(const string &retryPrompt = "Invalid input. Enter number: ")
{
    int value;
    while (!(cin >> value))
    {
        if (cin.eof())
        {
            cout << "\nInput closed. Exiting." << endl;
            exit(0);
        }
        cout << retryPrompt;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return value;
}

string processPassword(string password)
{
    string processed = "";
    for (char c : password)
        processed += char(c ^ 7);
    return processed;
}

class Question
{
private:
    int questionID;
    string questionText;
    string optionA, optionB, optionC, optionD;
    char correctAnswer;

public:
    Question() : questionID(0), correctAnswer('A') {}
    Question(int id, string text, string a, string b, string c, string d, char correct)
        : questionID(id), questionText(text), optionA(a), optionB(b), optionC(c), optionD(d), correctAnswer(correct) {}

    void setQuestion(int id, string text, string a, string b, string c, string d, char correct)
    {
        questionID = id;
        questionText = text;
        optionA = a;
        optionB = b;
        optionC = c;
        optionD = d;
        correctAnswer = correct;
    }

    int getQuestionID() const { return questionID; }
    string getQuestionText() const { return questionText; }
    string getOptionA() const { return optionA; }
    string getOptionB() const { return optionB; }
    string getOptionC() const { return optionC; }
    string getOptionD() const { return optionD; }
    char getCorrectAnswer() const { return correctAnswer; }

    void displayQuestion() const
    {
        cout << "\nQ" << questionID << ": " << questionText << endl;
        cout << "  A) " << optionA << endl;
        cout << "  B) " << optionB << endl;
        cout << "  C) " << optionC << endl;
        cout << "  D) " << optionD << endl;
    }
};

class User
{
protected:
    string username;
    string password;
    string fullName;
    string userType;

public:
    User() : username(""), password(""), fullName(""), userType("") {}
    User(string uname, string pass, string fname, string type)
        : username(uname), password(pass), fullName(fname), userType(type) {}

    virtual ~User() {}

    string getUsername() const { return username; }
    string getFullName() const { return fullName; }
    string getUserType() const { return userType; }

    virtual bool login(string inputUsername, string inputPassword)
    {
        return (username == inputUsername && password == processPassword(inputPassword));
    }

    virtual void showMenu() = 0;
};

class Admin : public User
{
private:
    vector<Question> questionBank;

    void loadQuestions()
    {
        questionBank.clear();
        ifstream file("questions.txt");
        if (!file.is_open())
            return;

        int id;
        string text, a, b, c, d;
        char correct;

        while (file >> id)
        {
            file.ignore();
            getline(file, text);
            getline(file, a);
            getline(file, b);
            getline(file, c);
            getline(file, d);
            file >> correct;
            file.ignore();
            questionBank.push_back(Question(id, text, a, b, c, d, correct));
        }
        file.close();
    }

    void saveQuestions()
    {
        ofstream file("questions.txt");
        if (file.is_open())
        {
            for (const auto &q : questionBank)
            {
                file << q.getQuestionID() << endl;
                file << q.getQuestionText() << endl;
                file << q.getOptionA() << endl;
                file << q.getOptionB() << endl;
                file << q.getOptionC() << endl;
                file << q.getOptionD() << endl;
                file << q.getCorrectAnswer() << endl;
            }
            file.close();
        }
    }

    void addQuestion()
    {
        clearScreen();
        cout << "\n=== Add New Question ===" << endl;

        int id;
        cout << "Enter Question ID: ";
        while (!(cin >> id))
        {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(10000, '\n');
        }
        cin.ignore();

        for (const auto &q : questionBank)
        {
            if (q.getQuestionID() == id)
            {
                cout << "Question with this ID already exists!" << endl;
                return;
            }
        }

        string text, a, b, c, d;
        char correct;

        cout << "Enter Question Text: ";
        getline(cin, text);
        cout << "Enter Option A: ";
        getline(cin, a);
        cout << "Enter Option B: ";
        getline(cin, b);
        cout << "Enter Option C: ";
        getline(cin, c);
        cout << "Enter Option D: ";
        getline(cin, d);

        while (true)
        {
            cout << "Enter Correct Answer (A/B/C/D): ";
            cin >> correct;
            correct = toupper(correct);
            if (correct >= 'A' && correct <= 'D')
                break;
            cout << "Invalid choice!" << endl;
        }

        questionBank.push_back(Question(id, text, a, b, c, d, correct));
        saveQuestions();
        cout << "\nQuestion added successfully!" << endl;
    }

    void viewAllQuestions()
    {
        clearScreen();
        cout << "\n=== All Questions in Bank ===" << endl;

        if (questionBank.empty())
        {
            cout << "No questions in the bank!" << endl;
            return;
        }

        for (const auto &q : questionBank)
            q.displayQuestion();
    }

    void setPaper()
    {
        clearScreen();
        cout << "\n=== Set Paper (Create Exam) ===" << endl;

        if (questionBank.empty())
        {
            cout << "No questions available!" << endl;
            return;
        }

        string examName;
        cout << "Enter Exam Name: ";
        cin.ignore();
        getline(cin, examName);

        viewAllQuestions();

        cout << "\nEnter the Question IDs separated by commas: ";
        string idInput;
        getline(cin, idInput);

        stringstream ss(idInput);
        string idStr;
        vector<int> questionIDs;
        int numQuestions = 0;

        while (getline(ss, idStr, ','))
        {
            try
            {
                int id = stoi(idStr);
                bool found = false;
                for (const auto &q : questionBank)
                {
                    if (q.getQuestionID() == id)
                    {
                        found = true;
                        break;
                    }
                }
                if (found)
                {
                    questionIDs.push_back(id);
                    numQuestions++;
                }
            }
            catch (...)
            {
            }
        }

        if (questionIDs.empty())
        {
            cout << "No valid questions selected!" << endl;
            return;
        }

        ofstream file("exams.txt", ios::app);
        if (file.is_open())
        {
            file << examName << "|" << numQuestions << "|";
            for (size_t i = 0; i < questionIDs.size(); ++i)
            {
                file << questionIDs[i];
                if (i + 1 < questionIDs.size())
                    file << ",";
            }
            file << endl;
            file.close();
            cout << "\nExam '" << examName << "' set successfully!" << endl;
        }
    }

    void assignQuizToStudent()
    {
        clearScreen();
        cout << "\n=== Assign Exam to Student ===" << endl;

        ifstream examsFile("exams.txt");
        if (!examsFile.is_open())
        {
            cout << "No exams available!" << endl;
            return;
        }

        vector<string> exams;
        string line, examName;
        int count = 1;
        cout << "Available Exams:" << endl;
        while (getline(examsFile, line))
        {
            stringstream ss(line);
            getline(ss, examName, '|');
            cout << count << ". " << examName << endl;
            exams.push_back(examName);
            count++;
        }
        examsFile.close();

        if (exams.empty())
        {
            cout << "No exams found!" << endl;
            return;
        }

        cout << "Select Exam: ";
        int examChoice;
        cin >> examChoice;
        if (examChoice < 1 || examChoice > (int)exams.size())
        {
            cout << "Invalid selection." << endl;
            return;
        }
        string selectedExam = exams[examChoice - 1];

        ifstream studentsFile("students.txt");
        if (!studentsFile.is_open())
        {
            cout << "No students found." << endl;
            return;
        }

        vector<string> students;
        string username, pass, fname;
        count = 1;
        cout << "\nAvailable Students:" << endl;
        while (studentsFile >> username >> pass)
        {
            getline(studentsFile, fname);
            if (!fname.empty() && fname[0] == ' ')
                fname = fname.substr(1);
            cout << count << ". " << username << " (" << fname << ")" << endl;
            students.push_back(username);
            count++;
        }
        studentsFile.close();

        if (students.empty())
        {
            cout << "No students found." << endl;
            return;
        }

        cout << "Select Student: ";
        int studentChoice;
        cin >> studentChoice;
        if (studentChoice < 1 || studentChoice > (int)students.size())
        {
            cout << "Invalid selection." << endl;
            return;
        }
        string selectedStudent = students[studentChoice - 1];

        ofstream assignFile("assignments.txt", ios::app);
        if (assignFile.is_open())
        {
            assignFile << selectedStudent << "|" << selectedExam << endl;
            assignFile.close();
            cout << "\nAssigned successfully!" << endl;
        }
    }

    void viewStudentMarks()
    {
        clearScreen();
        cout << "\n=== All Students' Marks ===" << endl;

        ifstream file("marks.txt");
        if (!file.is_open())
        {
            cout << "No marks record found!" << endl;
            return;
        }

        string line;
        cout << left << setw(20) << "Student" << setw(30) << "Exam" << setw(10) << "Score" << endl;
        cout << string(60, '-') << endl;

        while (getline(file, line))
        {
            stringstream ss(line);
            string student, exam;
            int score;
            ss >> student >> exam >> score;
            cout << left << setw(20) << student << setw(30) << exam << setw(10) << score << endl;
        }
        file.close();
    }

    void addStudent()
    {
        clearScreen();
        cout << "\n--- Add Student ---" << endl;

        string username, password, fullName;
        cout << "Enter Student Username: ";
        cin >> username;

        ifstream check("students.txt");
        if (check.is_open())
        {
            string line, user, pass;
            while (check >> user >> pass)
            {
                getline(check, line);
                if (user == username)
                {
                    cout << "Student username already exists!" << endl;
                    check.close();
                    return;
                }
            }
            check.close();
        }

        cout << "Enter Password: ";
        cin >> password;
        cout << "Enter Full Name: ";
        cin.ignore();
        getline(cin, fullName);

        ofstream file("students.txt", ios::app);
        if (file.is_open())
        {
            file << username << " " << processPassword(password) << " " << fullName << endl;
            file.close();
            cout << "Student added successfully!" << endl;
        }
    }

    void removeStudent()
    {
        clearScreen();
        cout << "\n--- Remove Student ---" << endl;

        string username;
        cout << "Enter Student Username: ";
        cin >> username;

        ifstream file("students.txt");
        ofstream temp("temp.txt");

        if (file.is_open() && temp.is_open())
        {
            string line, user, pass, fname;
            bool found = false;

            while (file >> user >> pass)
            {
                getline(file, fname);
                if (user != username)
                    temp << user << " " << pass << fname << endl;
                else
                    found = true;
            }

            file.close();
            temp.close();

            remove("students.txt");
            rename("temp.txt", "students.txt");

            if (found)
                cout << "Student removed!" << endl;
            else
                cout << "Student not found!" << endl;
        }
    }

    void viewAllStudents()
    {
        clearScreen();
        cout << "\n--- All Students ---" << endl;

        ifstream file("students.txt");
        if (!file.is_open())
        {
            cout << "No students registered." << endl;
            return;
        }

        string user, pass, fname;
        cout << left << setw(20) << "Username" << "Full Name" << endl;
        cout << string(40, '-') << endl;

        while (file >> user >> pass)
        {
            getline(file, fname);
            if (!fname.empty() && fname[0] == ' ')
                fname = fname.substr(1);
            cout << left << setw(20) << user << fname << endl;
        }
        file.close();
    }

    void manageStudentAccounts()
    {
        int choice;
        while (true)
        {
            clearScreen();
            cout << "\n=== Manage Student Accounts ===" << endl;
            cout << "1. Add Student" << endl;
            cout << "2. Remove Student" << endl;
            cout << "3. View All Students" << endl;
            cout << "4. Back" << endl;
            cout << "Enter choice: ";
            cin >> choice;

            switch (choice)
            {
            case 1:
                addStudent();
                break;
            case 2:
                removeStudent();
                break;
            case 3:
                viewAllStudents();
                break;
            case 4:
                return;
            default:
                cout << "Invalid choice!" << endl;
            }
            if (choice != 4)
                pressEnterToContinue();
        }
    }

public:
    Admin(string uname, string pass, string fname) : User(uname, pass, fname, "Admin")
    {
        loadQuestions();
    }

    void showMenu() override
    {
        int choice;
        while (true)
        {
            clearScreen();
            cout << "\n--- Admin Menu ---" << endl;
            cout << "1. Add Question" << endl;
            cout << "2. Set Paper" << endl;
            cout << "3. Assign Exam" << endl;
            cout << "4. View Students' Marks" << endl;
            cout << "5. Manage Students" << endl;
            cout << "6. Logout" << endl;
            cout << "Enter choice: ";

            while (!(cin >> choice))
            {
                cout << "Invalid input. Enter number: ";
                cin.clear();
                cin.ignore(10000, '\n');
            }

            switch (choice)
            {
            case 1:
                addQuestion();
                break;
            case 2:
                setPaper();
                break;
            case 3:
                assignQuizToStudent();
                break;
            case 4:
                viewStudentMarks();
                break;
            case 5:
                manageStudentAccounts();
                break;
            case 6:
                return;
            default:
                cout << "Invalid choice!" << endl;
            }
            if (choice != 6)
                pressEnterToContinue();
        }
    }
};

class Student : public User
{
private:
    vector<Question> allQuestions;

    void loadAllQuestions()
    {
        allQuestions.clear();
        ifstream file("questions.txt");
        if (!file.is_open())
            return;
        int id;
        string text, a, b, c, d;
        char correct;
        while (file >> id)
        {
            file.ignore();
            getline(file, text);
            getline(file, a);
            getline(file, b);
            getline(file, c);
            getline(file, d);
            file >> correct;
            file.ignore();
            allQuestions.push_back(Question(id, text, a, b, c, d, correct));
        }
        file.close();
    }

    map<string, bool> getCompletedExams()
    {
        map<string, bool> completed;
        ifstream marksFile("marks.txt");
        if (marksFile.is_open())
        {
            string line;
            while (getline(marksFile, line))
            {
                stringstream ss(line);
                string user, exam;
                int score;
                ss >> user >> exam >> score;
                if (user == this->username)
                    completed[exam] = true;
            }
            marksFile.close();
        }
        return completed;
    }

    vector<string> getAssignedExams()
    {
        vector<string> assigned;
        ifstream assignFile("assignments.txt");
        if (assignFile.is_open())
        {
            string line;
            while (getline(assignFile, line))
            {
                stringstream ss(line);
                string user, exam;
                getline(ss, user, '|');
                getline(ss, exam);
                if (user == this->username)
                    assigned.push_back(exam);
            }
            assignFile.close();
        }
        return assigned;
    }

    void takeExam()
    {
        clearScreen();
        cout << "\n=== Take Exam ===" << endl;

        vector<string> assignedExams = getAssignedExams();
        map<string, bool> completedExams = getCompletedExams();

        vector<string> pendingExams;
        for (const string &exam : assignedExams)
            if (completedExams.find(exam) == completedExams.end())
                pendingExams.push_back(exam);

        if (pendingExams.empty())
        {
            cout << "You have no pending exams." << endl;
            return;
        }

        cout << "Your pending exams:" << endl;
        for (size_t i = 0; i < pendingExams.size(); ++i)
            cout << (i + 1) << ". " << pendingExams[i] << endl;

        cout << "Select Exam (0 to cancel): ";
        int choice;
        cin >> choice;
        if (choice <= 0 || choice > (int)pendingExams.size())
            return;

        string selectedExamName = pendingExams[choice - 1];

        ifstream examsFile("exams.txt");
        if (!examsFile.is_open())
        {
            cout << "Cannot open exams file." << endl;
            return;
        }

        string line, examData;
        while (getline(examsFile, line))
        {
            stringstream ss(line);
            string name;
            getline(ss, name, '|');
            if (name == selectedExamName)
            {
                examData = line;
                break;
            }
        }
        examsFile.close();

        if (examData.empty())
        {
            cout << "Error: Exam not found." << endl;
            return;
        }

        stringstream ss(examData);
        string segment, questionIDsStr;
        int numQuestions;

        getline(ss, segment, '|');
        getline(ss, segment, '|');
        numQuestions = stoi(segment);
        getline(ss, questionIDsStr);

        loadAllQuestions();
        if (allQuestions.empty())
        {
            cout << "Question bank empty." << endl;
            return;
        }

        vector<Question> examQuestions;
        stringstream idStream(questionIDsStr);
        string idStr;
        while (getline(idStream, idStr, ','))
        {
            int id = stoi(idStr);
            for (const auto &q : allQuestions)
                if (q.getQuestionID() == id)
                    examQuestions.push_back(q);
        }

        int score = 0;
        cout << "\n========== " << selectedExamName << " ==========" << endl;
        cout << "Total Questions: " << numQuestions << endl;
        cout << "Time limit: " << numQuestions << " minutes" << endl;

        time_t startTime = time(0);
        time_t timeLimit = numQuestions * 60;

        for (size_t i = 0; i < examQuestions.size(); ++i)
        {
            time_t currentTime = time(0);
            if (currentTime - startTime > timeLimit)
            {
                cout << "\nTime's up!" << endl;
                break;
            }

            examQuestions[i].displayQuestion();
            cout << "Your answer (A/B/C/D): ";
            char answer;
            cin >> answer;
            answer = toupper(answer);

            if (answer == examQuestions[i].getCorrectAnswer())
            {
                score++;
                cout << "Correct!" << endl;
            }
            else
            {
                cout << "Wrong! Correct: " << examQuestions[i].getCorrectAnswer() << endl;
            }
        }

        cout << "\n========== RESULTS ==========" << endl;
        cout << "Your Score: " << score << "/" << numQuestions << endl;
        double percentage = (double)score / numQuestions * 100;
        cout << "Percentage: " << fixed << setprecision(2) << percentage << "%" << endl;

        ofstream marksFile("marks.txt", ios::app);
        if (marksFile.is_open())
        {
            marksFile << this->username << " " << selectedExamName << " " << score << endl;
            marksFile.close();
        }
    }

    void viewMarks()
    {
        clearScreen();
        cout << "\n=== View My Marks ===" << endl;

        ifstream file("marks.txt");
        if (!file.is_open())
        {
            cout << "No marks record found!" << endl;
            return;
        }

        string line;
        cout << left << setw(30) << "Exam" << setw(10) << "Score" << endl;
        cout << string(40, '-') << endl;

        bool found = false;
        while (getline(file, line))
        {
            stringstream ss(line);
            string student, exam;
            int score;
            ss >> student >> exam >> score;
            if (student == this->username)
            {
                cout << left << setw(30) << exam << setw(10) << score << endl;
                found = true;
            }
        }
        file.close();

        if (!found)
            cout << "You have not attempted any exams yet." << endl;
    }

    void viewAssignedExams()
    {
        clearScreen();
        cout << "\n=== View Assigned Exams ===" << endl;

        vector<string> assignedExams = getAssignedExams();
        if (assignedExams.empty())
        {
            cout << "No exams assigned to you." << endl;
            return;
        }

        map<string, bool> completedExams = getCompletedExams();

        cout << left << setw(30) << "Exam Name" << "Status" << endl;
        cout << string(40, '-') << endl;

        for (const string &exam : assignedExams)
        {
            string status = "Pending";
            if (completedExams.find(exam) != completedExams.end())
                status = "Completed";
            cout << left << setw(30) << exam << status << endl;
        }
    }

public:
    Student(string uname, string pass, string fname) : User(uname, pass, fname, "Student") {}

    void showMenu() override
    {
        int choice;
        while (true)
        {
            clearScreen();
            cout << "\n--- Student Menu ---" << endl;
            cout << "1. Take Exam" << endl;
            cout << "2. View My Marks" << endl;
            cout << "3. View Assigned Exams" << endl;
            cout << "4. Logout" << endl;
            cout << "Enter choice: ";

            while (!(cin >> choice))
            {
                cout << "Invalid input. Enter number: ";
                cin.clear();
                cin.ignore(10000, '\n');
            }

            switch (choice)
            {
            case 1:
                takeExam();
                break;
            case 2:
                viewMarks();
                break;
            case 3:
                viewAssignedExams();
                break;
            case 4:
                return;
            default:
                cout << "Invalid choice!" << endl;
            }
            if (choice != 4)
                pressEnterToContinue();
        }
    }
};

class AuthenticationManager
{
private:
    Admin adminUser;
    vector<Student> students;

public:
    AuthenticationManager() : adminUser("admin", processPassword("admin123"), "Admin User")
    {
        loadStudents();
    }

    void loadStudents()
    {
        students.clear();
        ifstream file("students.txt");
        if (file.is_open())
        {
            string username, password, fullName;
            while (file >> username >> password)
            {
                getline(file, fullName);
                if (!fullName.empty() && fullName[0] == ' ')
                    fullName = fullName.substr(1);
                students.push_back(Student(username, password, fullName));
            }
            file.close();
        }
    }

    User *authenticateUser(string userType, string username, string password)
    {
        if (userType == "1")
        {
            if (adminUser.login(username, password))
                return &adminUser;
        }
        else if (userType == "2")
        {
            loadStudents();
            for (auto &student : students)
                if (student.login(username, password))
                    return &student;
        }
        return nullptr;
    }
};

int main()
{
    srand(static_cast<unsigned int>(time(0)));
    AuthenticationManager authManager;
    string userChoice;

    while (true)
    {
        clearScreen();
        cout << "\n=== Online Examination System ===" << endl;
        cout << "1. Admin Login" << endl;
        cout << "2. Student Login" << endl;
        cout << "3. Exit" << endl;
        cout << "Choice: ";
        cin >> userChoice;

        if (userChoice == "3")
        {
            cout << "Thank you for using the system!" << endl;
            break;
        }

        if (userChoice != "1" && userChoice != "2")
        {
            cout << "Invalid choice!" << endl;
            pressEnterToContinue();
            continue;
        }

        string username, password;
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        User *user = authManager.authenticateUser(userChoice, username, password);

        if (user != nullptr)
        {
            cout << "\nLogin successful. Welcome, " << user->getFullName() << endl;
            pressEnterToContinue();
            user->showMenu();
        }
        else
        {
            cout << "\nInvalid credentials!" << endl;
            pressEnterToContinue();
        }
    }

    return 0;
}
