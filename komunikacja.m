%Skrypt do synchronizacji czasu
%Wysy³amy zapytanie do serwera
%Otrzymujemy 11 bajtów (10 znaków + '\0'), które wklejamy bezpoœrednio do
%Windowsowej komendy 'time'

%must be run as administrator ! 
%if not try runas /user:USERNAME(Piotr) notepad
%ip address wil be given in the future
%% init
time=0;
time_old=0;
day_old=0;
clear all; %uwazajcie tu!

PORT_NUMBER = 8008;
IP = '192.168.2.2'; %ip serwera
ID = 'E' %identyfikator uzytkownika: E- elektrocieplowania, W- Wymiennik, R- Regulator, B- budynek
%nowa wersja matlaba
%t = tcpip(IP, PORT_NUMBER,'NetworkRole', 'client'); 

%stara wersja matlaba
t = tcpip(IP, PORT_NUMBER,'RemoteHost')

%% Program do uruchomienia symulacji
while(1);
    
    %% oczekiwanie na start symulacji
    wait=1;
    while(wait)
        fopen(t);
        fwrite(t, [ID,'!KeepAlive']);
        [A, COUNT] = fread(t, 12);
        if(COUNT > 0)
            text = char(A.')
        end
        fclose(t);
        if(strcmp(text(1:3),'sta')) %%oczekiwanie na start symulacji
            wait=0;
        end
        pause(0.2); % okres co jaki wysy³ana jest kolejna komenda keepalive
    end
    
    %% Odebranie czasu z serwera
    fopen(t);
    fwrite(t, [ID,'!GetTime']);
    [A, COUNT] = fread(t, 12);
    if(COUNT > 0)
        text = char(A.')
    end
    fclose(t);

%Parsowanie danych o czasie otrzymanych z serwera:
    time=str2num(text(4:5))*3600+60*str2num(text(7:8));
    day=str2num(text(1:2));

    %% miejsce na odebranie danych z serwera i sparsowanie

    fopen(t);
    fwrite(t, [ID,'!GetParams']);
    [A, COUNT] = fread(t, 12);
    if(COUNT > 0)
        text = char(A.')
    end
    fclose(t);
    
    % przyk³adowe parsowanie odebranych danych
    %E
    Tpm=str2num(text(2:4))+0.01*str2num(text(6:7));
    if(text(1)=='-')
        Tpm=-Tpm;
    end
    %W
%     Tpco=str2num(text(2:4))+0.01*str2num(text(6:7));
%     if(text(1)=='-')
%         Tpco=-Tpco;
%     end
%     Fzm=str2num(text(9:11))+0.0001*str2num(text(13:16));
%     if(text(8)=='-')
%         Fzm=-Fzm;
%     end
%     Tzm=str2num(text(19:21))+0.01*str2num(text(23:24));
%     if(text(18)=='-')
%         Tzm=-Tzm;
%     end
    %R
%     Tzco=str2num(text(2:4))+0.01*str2num(text(6:7));
%     if(text(1)=='-')
%         Tzco=-Tzco;
%     end
%     To=str2num(text(9:11))+0.01*str2num(text(13:14));
%     if(text(8)=='-')
%         To=-To;
%     end
    %B
%     Tzco=str2num(text(2:4))+0.01*str2num(text(6:7));
%     if(text(1)=='-')
%         Tzco=-Tzco;
%     end
%     To=str2num(text(9:11))+0.01*str2num(text(13:14));
%     if(text(8)=='-')
%         To=-To;
%     end


    %komenda str2num zamienia licze zapisana w string na liczbe
    
    %% miejsce na uruchomienie symulacji
    if(day_old==day or day_old=0)
        sim_time=time-time_old;
    else if day_old~=day
            sim_time=time+86400-time_old; %przejscie pomiedzy dniami
        end
%wywolanie Waszego modelu

    %sim('model',time-time_old);

%dane zwrocone z waszej symulacji powinny pojawic sie w workspace, tutaj
%dane do testow
To=20.4;
Tzm=-23.93;
    %% mijesce na wyslanie danych na serwer: sparsowanie 

    % stworzenie ramki do wyslania 
    % gotowe wysylanie danych dla kazdego 
    Data=sprintf('%+03.2!f%03.2f',To,Tzm); %E
    %Data=sprintf('%+03.2!f%3.2f',Tpm,Tzco); %W
    %Data=sprintf('%+03.2f',Tpco); %B
    %Data=sprintf('%+03.4f',Fzm); %R

    fopen(t);
    fwrite(t, [ID,'!SendData']);
    fwrite(t, [ID,'!',Data]);
    fclose(t);
        
    % ustalenie starego czasu
    time_old=time;
    day_old=day;
    
    
end

delete(t)
