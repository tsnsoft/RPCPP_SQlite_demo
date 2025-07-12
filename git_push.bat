@echo off
cd /d "%~dp0"

echo Adding all changes...
git add .

:: Capture date and time
for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set "datetime=%%I"
if not defined datetime (
    echo Error: Could not retrieve date and time.
    set "commit_msg=rev. fallback_commit"
) else (
    set "commit_msg=rev. %datetime:~0,8%_%datetime:~8,6%"
)

echo Creating commit with message: %commit_msg%
git commit -m "%commit_msg%"

echo Pushing to GitHub...
git push origin main

echo Done!