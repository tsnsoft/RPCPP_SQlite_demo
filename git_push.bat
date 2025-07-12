@echo off
cd /d "%~dp0"

echo Adding all changes...
git add .

:: Try to capture date and time using wmic
for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value 2^>nul') do set "datetime=%%I"
if not defined datetime (
    echo Warning: wmic failed, using fallback date/time method.
    :: Fallback: Normalize date and time
    :: Handle common date formats (MM/DD/YYYY, DD-MM-YYYY, DD.MM.YYYY, etc.)
    for /f "tokens=1-3 delims=/.- " %%A in ("%date%") do (
        :: Try to detect format: DD MM YYYY or MM DD YYYY
        set "day=%%A"
        set "month=%%B"
        set "year=%%C"
    )
    :: Normalize to YYYYMMDD
    if "!year:~2!"=="" (
        :: Assume DD MM YYYY
        set "datepart=!year!!month!!day!"
    ) else (
        :: Assume MM DD YYYY
        set "datepart=!year!!month!!day!"
    )
    :: Handle time (HH:MM:SS)
    for /f "tokens=1-3 delims=:" %%D in ("%time%") do (
        set "hour=%%D"
        set "minute=%%E"
        set "second=%%F"
    )
    :: Remove leading space in hour (e.g., " 1" for 01:XX:XX)
    set "hour=!hour: =!"
    set "timepart=!hour!!minute!!second!"
    set "commit_msg=rev. !datepart:~0,8!_!timepart:~0,6!"
) else (
    set "commit_msg=rev. %datetime:~0,8%_%datetime:~8,6%"
)

:: Debug output
echo Debug: wmic datetime=%datetime%
echo Debug: date=%date%, time=%time%
echo Debug: datepart=%datepart%, timepart=%timepart%
echo Debug: commit_msg=%commit_msg%
echo Creating commit with message: %commit_msg%
git commit -m "%commit_msg%"

echo Pushing to GitHub...
git push origin main

echo Done!