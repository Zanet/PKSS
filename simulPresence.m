%Keep alive from all of the hosts

clear all;
PORT_NUMBER = 8008;
IP = '127.0.0.1';
t = tcpip(IP, PORT_NUMBER, 'NetworkRole', 'client'); 
fopen(t);
fwrite(t, 'E!KeepAlive');
fclose(t);

fopen(t);
fwrite(t, 'B!KeepAlive');
fclose(t);

fopen(t);
fwrite(t, 'R!KeepAlive');
fclose(t);

fopen(t); 
fwrite(t, 'W!KeepAlive');
fclose(t);
