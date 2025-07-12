@echo off
cd /d "%~dp0"

echo Adding all changes...
git add .

:: Проверка среды выполнения (Git Bash/WSL или Windows)
for /f "tokens=*" %%I in ('echo %OSTYPE%') do set "ostype=%%I"
if "%ostype%"=="msys" (
    echo Debug: Detected Git Bash/WSL, using Unix date command.
    for /f "tokens=*" %%I in ('date +%%Y%%m%%d_%%H%%M%%S') do set "commit_msg=rev. %%I"
) else (
    echo Debug: Using Windows date/time.
    :: Обработка различных форматов даты (MM/DD/YYYY, DD-MM-YYYY, DD.MM.YYYY и т.д.)
    for /f "tokens=1-3 delims=/.- " %%A in ("%date%") do (
        set "day=%%A"
        set "month=%%B"
        set "year=%%C"
    )
    for /f "tokens=1-3 delims=:" %%D in ("%time%") do (
        set "hour=%%D"
        set "minute=%%E"
        set "second=%%F"
    )
    :: Удаление пробела в часах (например, " 1" → "01")
    set "hour=%hour: =%"
    :: Нормализация в YYYYMMDD_HHMMSS
    set "datepart=%year%%month%%day%"
    set "timepart=%hour%%minute%%second%"
    set "commit_msg=rev. %datepart:~-8%_%timepart:~0,6%"
)

:: Отладочный вывод
echo Debug: ostype=%ostype%
echo Debug: date=%date%, time=%time%
echo Debug: datepart=%datepart%, timepart=%timepart%
echo Debug: commit_msg=%commit_msg%
echo Creating commit with message: %commit_msg%
git commit -m "%commit_msg%"

echo Pushing to GitHub...
git push origin main

echo Done!