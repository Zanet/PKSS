%Skrypt do synchronizacji czasu
%Wysy³amy zapytanie do serwera
%Otrzymujemy 11 bajtów (10 znaków + '\0'), które wklejamy bezpoœrednio do
%Windowsowej komendy 'time'

%must be run as administrator ! 
%if not try runas /user:USERNAME(Piotr) notepad
%ip address wil be given in the future

clear all; 

PORT_NUMBER = 32001;
IP = '192.168.2.4';

t = tcpip(IP, PORT_NUMBER, 'NetworkRole', 'client'); 
fopen(t);
%fwrite(t,'!getTime');

%while
 fwrite(t, '!E!S123');


[A, COUNT] = fread(t, 12)

if(COUNT > 0)
    text = char(A.')
    fclose(t);
    delete(t)

%     command = 'echo HH:MM:00 PM | time';
%     hour = text(1:2);
%     minute = text(4:5);
%     command = strrep(command, 'HH', hour);
%     command = strrep(command, 'MM', minute);
% 
%     system(command);
%else
%    echo 'Receive error! Size mismatch.'
end