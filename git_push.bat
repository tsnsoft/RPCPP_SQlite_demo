@echo off
cd /d "%~dp0"

echo Добавление всех изменений...
git add .

:: Пол rakaia date and time
for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set "datetime=%%I"
if not defined datetime (
    echo Ошибка: Не удалось получить дату и время.
    set "commit_msg=rev. fallback_commit"
) else (
    set "commit_msg=rev. %datetime:~0,8%_%datetime:~8,6%"
)

echo Создание коммита с сообщением: %commit_msg%
git commit -m "%commit_msg%"

echo Отправка на GitHub...
git push origin main

echo Готово!