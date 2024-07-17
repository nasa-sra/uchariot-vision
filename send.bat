@echo off
set host=%1
set src_dir=%~dp0src
set include_dir=%~dp0include
set remote_dir=uchariot@%host%:~/uchariot-vision/

scp -r "%src_dir%" "%remote_dir%"

if "%2" neq "" (
    echo sending /include
    scp -r "%include_dir%" "%remote_dir%/include"
)

:: Uncomment the line below if you need to send the CMakeLists.txt file
:: scp "%~dp0CMakeLists.txt" "%remote_dir%/CMakeLists.txt"