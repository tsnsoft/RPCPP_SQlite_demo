@echo off
cd /d "%~dp0"

echo Adding all changes...
git add .

:: Получаем дату и время из WMIC
for /f "tokens=2 delims==." %%I in ('wmic os get localdatetime /value') do (
    set datetime=%%I
)

setlocal enabledelayedexpansion

:: Извлекаем компоненты даты и времени
set year=!datetime:~0,4!
set month=!datetime:~4,2!
set day=!datetime:~6,2!
set hour=!datetime:~8,2!
set minute=!datetime:~10,2!

:: Формируем сообщение коммита в читаемом виде
set commit_msg=rev. !day!.!month!.!year! !hour!:!minute!

echo Creating commit with message "!commit_msg!"...
git commit -m "!commit_msg!"

echo Pushing to GitHub...
git push origin main

echo Done!
