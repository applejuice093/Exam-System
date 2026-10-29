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

// Parses "3, 1,2,x,3" into {3, 1, 2}: invalid entries and repeats are
// skipped, and the first-seen order is kept.
inline std::vector<int> parseIdList(const std::string &csv)
{
    std::vector<int> ids;
    std::stringstream ss(csv);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        int id;
        if (!parseIntStrict(item, id))
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
