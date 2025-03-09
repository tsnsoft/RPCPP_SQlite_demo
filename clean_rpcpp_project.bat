@echo off

REM Определение шаблонов файлов для удаления
set FILE_PATTERNS=*.o *.exe *.bookmarks *.debug *.Manifest *.layout *_private.* *.win

REM Удаление указанных шаблонов файлов
for %%f in (%FILE_PATTERNS%) do (
    del /Q %%f >nul 2>&1
)

echo Cleaning of project is completed.
