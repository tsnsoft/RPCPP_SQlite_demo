@echo off
cd /d "%~dp0"

echo Adding all changes...
git add .

:: Capture date and time in a reliable format (YYYYMMDD_HHMMSS)
for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set "datetime=%%I"
set "commit_msg=rev. %datetime:~0,8%_%datetime:~8,6%"

echo Creating commit...
git commit -m "%commit_msg%"

echo Pushing to GitHub...
git push origin main

echo Done!