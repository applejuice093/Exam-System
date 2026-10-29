# Online Examination System (C++)

A console app for running multiple-choice exams. An admin builds a question bank,
sets exam papers and assigns them to students; students log in, take timed exams
and see their marks. All data is stored in plain text files next to the program.

## Features

**Admin**
- Add multiple-choice questions (options A-D)
- Set an exam paper from chosen question IDs
- Assign exams to students
- View students' marks
- Add, remove and list student accounts

**Student**
- Take an assigned exam (time limit of one minute per question)
- View marks and pending exams

## Build and run

You need a C++17 compiler (g++ or clang++).

```bash
make              # builds ./exam_system
./exam_system
```

Without make: `g++ -std=c++17 -O2 -o exam_system main.cpp`.
On Windows (MinGW or Code::Blocks) the same source builds `exam_system.exe`.

## Default login

The built-in admin account is username `admin`, password `admin123`.
Create student accounts from **Admin > Manage Students**. Change the admin
password in `AuthenticationManager` before using this anywhere real.

## Data files

Created in the working folder as you use the app:

| File | Contents |
| --- | --- |
| `questions.txt` | Question bank |
| `exams.txt` | `examName\|numQuestions\|id1,id2,...` |
| `assignments.txt` | `student\|examName`, one line per assignment |
| `students.txt` | `username obfuscatedPassword Full Name` |
| `marks.txt` | `student\|examName\|score` |

Passwords are only XOR-obfuscated, not securely hashed.

Older builds wrote `marks.txt` as `student exam score` (space-separated), which
broke for exam names with spaces. Those lines are still read; new results use
the `|` format. Exam names and usernames can't contain `|`.

## Rules the app enforces

- An exam is scored against the questions that still exist in the bank, and an
  answer typed after the time limit is not counted.
- Answers must be A-D; anything else re-prompts.
- Exam names must be unique; assigning the same exam twice to a student is refused.
- Removing a student also removes their pending assignments (marks are kept).
- Passwords that the XOR step would turn into whitespace (such as ones with an
  apostrophe) are rejected because they can't be stored in `students.txt`.

## Code layout

| File | Purpose |
| --- | --- |
| `main.cpp` | Menus, file I/O and the `Admin` / `Student` / `AuthenticationManager` classes |
| `exam_utils.h` | Pure helpers (parsing, validation, password obfuscation) shared with the tests |

## Tests

```bash
make test        # unit tests, then the end-to-end smoke test
make unit        # unit tests only
```

- `tests/unit_tests.cpp` checks the helpers in `exam_utils.h` (no framework needed).
- `tests/smoke_test.sh` drives the real menus through stdin in a temporary folder:
  admin login, adding questions, setting and assigning an exam, and a student
  taking it.

Without make:

```bash
g++ -std=c++17 -o tests/unit_tests tests/unit_tests.cpp && ./tests/unit_tests
g++ -std=c++17 -o exam_system main.cpp && ./tests/smoke_test.sh
```
