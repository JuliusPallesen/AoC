@echo off
setlocal EnableDelayedExpansion

REM Set the path to the template file
set "templateFilePath=%~dp0template.cpp"

REM Loop through days 1 to 25
for /L %%i in (1, 1, 25) do (
    REM Create the folder
    set "folderName=Day%%i"
    set "folderPath=%~dp0!folderName!"
    mkdir "!folderPath!"

    REM Copy the template.cpp file and rename it
    set "destinationFilePath=!folderPath!\d%%i.cpp"
    copy "!templateFilePath!" "!destinationFilePath!"

    REM Create an empty "input" file in each folder
    set "inputFilePath=!folderPath!\input"
    type nul > "!inputFilePath!"
)