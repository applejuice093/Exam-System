// Small, I/O-free helpers used by main.cpp. Kept in a header so they can be
// unit tested (tests/unit_tests.cpp) without driving the interactive menus.
#pragma once

#include <cctype>
#include <climits>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// Returns s without leading/trailing whitespace.
inline std::string trim(const std::string &s)
{
    std::size_t start = 0, end = s.size();
    while (start < end && std::isspace(static_cast<unsigned char>(s[start])))
        ++start;
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
        --end;
    return s.substr(start, end - start);
}

// Parses a whole (trimmed) string as an int. Unlike stoi it rejects trailing
// junk such as "12abc" and never throws.
inline bool parseIntStrict(const std::string &text, int &out)
{
    std::string s = trim(text);
    if (s.empty())
        return false;
    try
    {
        std::size_t used = 0;
        long value = std::stol(s, &used);
        if (used != s.size() || value < INT_MIN || value > INT_MAX)
            return false;
        out = static_cast<int>(value);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

// Parses "3, 1,2,x,3" into {3, 1, 2}: invalid entries, non-positive IDs
// and repeats are skipped, and the first-seen order is kept.
inline std::vector<int> parseIdList(const std::string &csv)
{
    std::vector<int> ids;
    std::stringstream ss(csv);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        int id;
        // Question IDs must be positive; 0 and negatives are ignored.
        if (!parseIntStrict(item, id) || id <= 0)
            continue;
        bool seen = false;
        for (int existing : ids)
            if (existing == id)
                seen = true;
        if (!seen)
            ids.push_back(id);
    }
    return ids;
}

// One line of marks.txt.
struct MarkRecord
{
    std::string student;
    std::string exam;
    int score = 0;
};

// Formats a record as "student|exam|score". The '|' separator lets exam
// names contain spaces (the old space-separated format could not).
inline std::string formatMarkLine(const MarkRecord &r)
{
    return r.student + "|" + r.exam + "|" + std::to_string(r.score);
}

// Parses "student|exam|score", falling back to the legacy
// "student exam score" layout written by older builds. Returns false for
// blank or malformed lines.
inline bool parseMarkLine(const std::string &line, MarkRecord &out)
{
    std::string s = trim(line);
    if (s.empty())
        return false;

    std::size_t first = s.find('|');
    if (first != std::string::npos)
    {
        std::size_t last = s.rfind('|');
        if (last == first)
            return false;
        MarkRecord r;
        r.student = trim(s.substr(0, first));
        r.exam = trim(s.substr(first + 1, last - first - 1));
        if (r.student.empty() || r.exam.empty() || !parseIntStrict(s.substr(last + 1), r.score))
            return false;
        out = r;
        return true;
    }

    std::stringstream ss(s);
    MarkRecord r;
    std::string extra;
    if (!(ss >> r.student >> r.exam >> r.score) || (ss >> extra))
        return false;
    out = r;
    return true;
}

// True if s can be stored as one field of the '|'-separated data files:
// non-blank, and free of the '|' separator and line breaks.
inline bool isValidRecordField(const std::string &s)
{
    if (trim(s).empty())
        return false;
    return s.find_first_of("|\r\n") == std::string::npos;
}

// NOTE: this XOR step only obfuscates passwords in students.txt; it is not
// real hashing. A production system should use a salted hash (e.g. bcrypt).
inline std::string processPassword(const std::string &password)
{
    std::string processed;
    for (char c : password)
        processed += char(c ^ 7);
    return processed;
}

// students.txt is whitespace-separated, so the obfuscated password must not
// contain whitespace. XOR 7 turns an apostrophe (0x27) into a space, which
// would split the record and lock the student out.
inline bool isStorablePassword(const std::string &password)
{
    if (password.empty())
        return false;
    for (char c : processPassword(password))
        if (std::isspace(static_cast<unsigned char>(c)) || c == '\0')
            return false;
    return true;
}

// One line of assignments.txt: "student|examName".
struct AssignmentRecord
{
    std::string student;
    std::string exam;
};

inline std::string formatAssignmentLine(const AssignmentRecord &r)
{
    return r.student + "|" + r.exam;
}

// Accepts a trailing '\r' (CRLF files) because trim strips it.
inline bool parseAssignmentLine(const std::string &line, AssignmentRecord &out)
{
    std::string s = trim(line);
    if (s.empty())
        return false;
    std::size_t bar = s.find('|');
    if (bar == std::string::npos)
        return false;
    AssignmentRecord r;
    r.student = trim(s.substr(0, bar));
    r.exam = trim(s.substr(bar + 1));
    // A second '|' would mean the exam name isn't a single field.
    if (r.student.empty() || r.exam.empty() || r.exam.find('|') != std::string::npos)
        return false;
    out = r;
    return true;
}

// One line of exams.txt: "examName|declaredCount|id1,id2,..."
struct ExamRecord
{
    std::string name;
    int declaredCount = 0;
    std::vector<int> questionIds;
};

inline std::string formatExamLine(const ExamRecord &r)
{
    std::string ids;
    for (std::size_t i = 0; i < r.questionIds.size(); ++i)
    {
        if (i)
            ids += ",";
        ids += std::to_string(r.questionIds[i]);
    }
    return r.name + "|" + std::to_string(static_cast<int>(r.questionIds.size())) + "|" + ids;
}

inline bool parseExamLine(const std::string &line, ExamRecord &out)
{
    std::string s = trim(line);
    if (s.empty())
        return false;
    std::size_t first = s.find('|');
    if (first == std::string::npos)
        return false;
    std::size_t second = s.find('|', first + 1);
    if (second == std::string::npos)
        return false;
    ExamRecord r;
    r.name = trim(s.substr(0, first));
    if (r.name.empty() || !parseIntStrict(s.substr(first + 1, second - first - 1), r.declaredCount))
        return false;
    r.questionIds = parseIdList(s.substr(second + 1));
    out = r;
    return true;
}

// One line of students.txt: "username obfuscatedPassword Full Name".
// The password field cannot contain whitespace (see isStorablePassword).
struct StudentRecord
{
    std::string username;
    std::string password;
    std::string fullName;
};

inline std::string formatStudentLine(const StudentRecord &r)
{
    return r.username + " " + r.password + " " + r.fullName;
}

inline bool parseStudentLine(const std::string &line, StudentRecord &out)
{
    std::string s = trim(line);
    if (s.empty())
        return false;
    std::stringstream ss(s);
    StudentRecord r;
    if (!(ss >> r.username >> r.password))
        return false;
    std::string rest;
    std::getline(ss, rest);
    r.fullName = trim(rest);
    if (r.username.empty() || r.password.empty() || r.fullName.empty())
        return false;
    out = r;
    return true;
}

// Usernames are a single students.txt token: no whitespace or '|',
// 1-32 characters, and not the reserved built-in admin login.
inline bool isValidUsername(const std::string &s)
{
    std::string u = trim(s);
    if (u.empty() || u.size() > 32)
        return false;
    if (u.find_first_of(" \t\r\n|") != std::string::npos)
        return false;
    std::string lower;
    lower.reserve(u.size());
    for (char c : u)
        lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    if (lower == "admin")
        return false;
    return true;
}

// Reject empty, unstorable, or very short / very long passwords.
inline bool isAcceptablePassword(const std::string &password)
{
    if (password.size() < 3 || password.size() > 64)
        return false;
    return isStorablePassword(password);
}
