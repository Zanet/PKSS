@ECHO OFF

IF "%2"=="" (echo "Usage: set_time.bat HH:MM AM/PM"
) ELSE (
echo %1 %2 
SLEEP 2
echo "Ustawiam czas po 2 sekundach")
 :: time %1 %2
 :: )