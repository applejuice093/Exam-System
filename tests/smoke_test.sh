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
echo "smoke test passed"
