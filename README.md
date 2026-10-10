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
| `exams.txt` | `examName|numQuestions|id1,id2,...` |
| `assignments.txt` | Which student has which exam |
| `students.txt` | Student usernames, obfuscated passwords and names |
| `marks.txt` | Exam results |

Passwords are only XOR-obfuscated, not securely hashed.

## Tests

```bash
make test
```

Runs `tests/smoke_test.sh`, which drives the menus through stdin in a temporary folder.
