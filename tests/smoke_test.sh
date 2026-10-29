#!/usr/bin/env bash
# Smoke test: drives the menus through stdin in a temp directory.
set -euo pipefail
BIN="$(cd "$(dirname "$0")/.." && pwd)/exam_system"
WORK="$(mktemp -d)"; trap 'rm -rf "$WORK"' EXIT
cd "$WORK"

# Admin login, a non-numeric menu choice (must re-prompt, not loop), logout, exit.
printf '1\nadmin\nadmin123\n\nabc\n6\n3\n' | timeout 5 "$BIN" > out.txt
grep -q "Welcome, Admin User" out.txt
grep -q "Invalid input. Enter number" out.txt
grep -q "Thank you for using the system" out.txt

# Wrong password is rejected.
printf '1\nadmin\nwrong\n\n3\n' | timeout 5 "$BIN" > out2.txt
grep -q "Invalid credentials" out2.txt

# Closing stdin must exit instead of hanging.
timeout 5 "$BIN" < /dev/null > /dev/null
# End-to-end: admin builds an exam whose name has spaces, adds a student and
# assigns the exam (twice, the second must be refused). Blank lines answer
# the "Press Enter to continue" prompts.
{
  printf '1\nadmin\nadmin123\n\n'
  printf '1\n1\nWhat is 2+2?\n3\n4\n5\n22\nB\n\n'           # Add Question 1
  printf '1\n2\nCapital of France?\nRome\nParis\nOslo\nBern\nb\n\n' # Add Question 2
  printf '2\nUnit Test 1\n1,2,1\n\n\n'                           # Set Paper
  printf '2\nUnit Test 1\n\n\n'                                   # duplicate name refused
  printf "5\n1\nbob\nit's\n\n\n"                               # unstorable password refused
  printf '1\nalice\npw1\nAlice Smith\n\n\n4\n\n'               # Add Student, back
  printf '3\n1\n1\n\n'                                            # Assign
  printf '3\n1\n1\n\n'                                            # Assign again
  printf '6\n3\n'
} | timeout 10 "$BIN" > admin.txt
grep -q "Question added successfully" admin.txt
grep -q "Exam 'Unit Test 1' set successfully" admin.txt
grep -q "already exists" admin.txt
grep -q "can't be stored" admin.txt
grep -q "Student added successfully" admin.txt
grep -q "Assigned successfully" admin.txt
grep -q "is already assigned" admin.txt
grep -q '^Unit Test 1|2|1,2$' exams.txt
[ "$(wc -l < assignments.txt)" -eq 1 ]

# Student takes the exam: an invalid letter re-prompts, one right, one wrong.
{
  printf '2\nalice\npw1\n\n'
  printf '1\n1\nx\nb\na\n\n'   # Take Exam
  printf '1\n\n'                   # nothing pending any more
  printf '2\n\n'                   # View My Marks
  printf '4\n3\n'
} | timeout 10 "$BIN" > student.txt
grep -q "Please enter A, B, C or D" student.txt
grep -q "Your Score: 1/2" student.txt
grep -q "You have no pending exams" student.txt
grep -q "Unit Test 1 *1" student.txt
grep -q '^alice|Unit Test 1|1$' marks.txt

echo "smoke test passed"
