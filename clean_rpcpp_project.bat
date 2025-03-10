@echo off
setlocal enabledelayedexpansion

for %%f in (*.o) do del /Q /F "%%f" >nul 2>&1
for %%f in (*.exe) do del /Q /F "%%f" >nul 2>&1
for %%f in (*.bookmarks) do del /Q /F "%%f" >nul 2>&1
for %%f in (*.debug) do del /Q /F "%%f" >nul 2>&1
for %%f in (*.Manifest) do del /Q /F "%%f" >nul 2>&1
for %%f in (*.layout) do del /Q /F "%%f" >nul 2>&1
for %%f in (*_private.*) do del /Q /F "%%f" >nul 2>&1
for %%f in (*.win) do del /Q /F "%%f" >nul 2>&1

echo Cleaning of project is completed.
