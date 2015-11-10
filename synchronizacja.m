%Skrypt do synchronizacji czasu
%Wysy³amy zapytanie do serwera
%Otrzymujemy 11 bajtów (10 znaków + '\0'), które wklejamy bezpoœrednio do
%Windowsowej komendy 'time'

%must be run as administrator ! 
%if not try runas /user:USERNAME(Piotr) notepad

t = tcpip('192.168.56.101',5001); 
fopen(t);
fwrite(t,'!getTime'); 
A = fread(t, 11); 
text = char(A.')
fclose(t);
delete(t)

command = strcat(['echo ' text ' | time'])      
        
system(command);        