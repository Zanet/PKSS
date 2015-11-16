@ECHO OFF

IF "%2"=="" (echo "Usage: set_time.bat HH:MM AM/PM"
) ELSE (
echo %1 %2
time %1 %2
)