%Keep alive from all of the hosts

clear all;
PORT_NUMBER = 8008;
IP = '127.0.0.1';
t = tcpip(IP, PORT_NUMBER, 'NetworkRole', 'client'); 

%Initial Data

fopen(t);
fwrite(t, 'E!SendData!020.1100!000.3010');
fclose(t);

fopen(t);
fwrite(t, 'R!SendData!101.2211');
fclose(t);

fopen(t);
fwrite(t, 'B!SendData!257.2333!-100.0');
fclose(t);

fopen(t);
fwrite(t, 'W!SendData!333.3133!444.1144');
fclose(t);











fopen(t);
fwrite(t, 'E!KeepAlive');
fclose(t);

fopen(t);
fwrite(t, 'B!KeepAlive');
fclose(t);

fopen(t);
fwrite(t, 'R!KeepAlive');
fclose(t);



%Tu juz powinni wszyscy byc znani -> dostajemy "Sta" i rozpoczynamy symulke

%KA
fopen(t); 
fwrite(t, 'W!KeepAlive');
fclose(t);

fopen(t); 
fwrite(t, 'B!KeepAlive');
fclose(t);

fopen(t); 
fwrite(t, 'R!KeepAlive');
fclose(t);

fopen(t);
fwrite(t, 'E!KeepAlive');
fclose(t);

% %GetTime
fopen(t);
fwrite(t, 'E!GetTime');
fclose(t);
% 
% fopen(t);
% fwrite(t, 'R!GetTime');
% fclose(t);
% 
% fopen(t);
% fwrite(t, 'B!GetTime');
% fclose(t);
% 
% fopen(t);
% fwrite(t, 'W!GetTime');
% fclose(t);
% 
% 
% %GetParams
% fopen(t);
% fwrite(t, 'E!GetParams');
% fclose(t);
% 
% fopen(t);
% fwrite(t, 'W!GetParams');
% fclose(t);
% 
% fopen(t);
% fwrite(t, 'B!GetParams');
% fclose(t);
% 
% 
% fopen(t);
% fwrite(t, 'R!GetParams');
% fclose(t);
% 
% %SendData
% 
% fopen(t);
% fwrite(t, 'E!SendData!13.456!14.00');
% fclose(t);
% 
% fopen(t);
% fwrite(t, 'R!SendData!88.14');
% fclose(t);
% 
% fopen(t);
% fwrite(t, 'B!SendData!76.56');
% fclose(t);
% 
% fopen(t);
% fwrite(t, 'W!SendData!99.456!174.00');
% fclose(t);
% 








