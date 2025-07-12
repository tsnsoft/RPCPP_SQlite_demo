@echo off
cd /d "%~dp0"

echo Adding all changes...
git add .

:: Try to capture date and time using wmic
for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value 2^>nul') do set "datetime=%%I"
if not defined datetime (
    echo Warning: wmic failed, using fallback date/time method.
    :: Fallback to date and time commands
    for /f "tokens=2 delims=." %%A in ("%date%") do set "datepart=%%A"
    for /f "tokens=1-2 delims=:" %%B in ("%time%") do set "timepart=%%B%%C"
    set "commit_msg=rev. %datepart:~-8%_%timepart:~0,6%"
) else (
    set "commit_msg=rev. %datetime:~0,8%_%datetime:~8,6%"
)

echo Creating commit with message: %commit_msg%
git commit -m "%commit_msg%"

echo Pushing to GitHub...
git push origin main

echo Done!