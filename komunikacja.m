%% init
clear all; %uwazajcie tu!

time=0;
time_old = 0;
day_old = 0;

PORT_NUMBER = 8008;
IP = 'localhost'; %ip serwera
ID = 'W' %identyfikator uzytkownika: E- elektrocieplowania, W- Wymiennik, R- Regulator, B- budynek
%nowa wersja matlaba
t = tcpip(IP, PORT_NUMBER,'NetworkRole', 'client'); 

%stara wersja matlaba
%t = tcpip(IP, PORT_NUMBER,'RemoteHost')

%% Program do uruchomienia symulacji
while(1)
    
    %% oczekiwanie na start symulacji
    wait=1;
    while(wait)
        fopen(t);
        fwrite(t, [ID,'!KeepAlive']);
        [A, COUNT] = fread(t, 4);
        if(COUNT > 0)
            text = char(A.');
        end
        fclose(t);
        if(strcmp(text(1:3),'sta')) %%oczekiwanie na start symulacji
            wait=0;
        end
        pause(0.2); % okres co jaki wysyłana jest kolejna komenda keepalive
    end
    
    %% Odebranie czasu z serwera
    fopen(t);
    fwrite(t, [ID,'!GetTime']);
    [A, COUNT] = fread(t, 9);
    if(COUNT > 0)
        text = char(A.')
    end
    fclose(t);

%Parsowanie danych o czasie otrzymanych z serwera:
    time=str2num(text(4:5))*3600+60*str2num(text(7:8))
    day=str2num(text(1:2))

    %% miejsce na odebranie danych z serwera i sparsowanie

    fopen(t);
    fwrite(t, [ID,'!GetParams']);
    
    %Tutaj uwaga: serwer na GetParams wysyla tekst o dlugosci
    % ilosc parametrow * DLUGOSC_REKORDU (10) + ilosc parametrow - 1
    %dla 3 parametrow mamy 32,
    %dla 2 parametrow mamy 21
    [A, COUNT] = fread(t, 32); % 3 parametry pobiera
    if(COUNT > 0)
        text = char(A.')
    end
    fclose(t);
    
    % przykładowe parsowanie odebranych danych
    %
%     Tpm=str2num(text(2:4))+0.01*str2num(text(6:7));
%     if(text(1)=='-')
%         Tpm=-Tpm;
%     end
   % W
    Tpco=str2num(text(2:4))+0.01*str2num(text(6:7));
    if(text(1)=='-')
        Tpco=-Tpco;
    end
    Fzm=str2num(text(9:11))+0.0001*str2num(text(13:16));
    if(text(8)=='-')
        Fzm=-Fzm;
    end
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
    if(day_old == day || day_old == 0)
        sim_time = time-time_old;
    else if day_old ~= day
            sim_time=time+86400-time_old; %przejscie pomiedzy dniami
        end
    end
%wywolanie Waszego modelu

    %sim('model',time-time_old);

%dane zwrocone z waszej symulacji powinny pojawic sie w workspace, tutaj
%dane do testow
%To=20.4;
%Tzm=-23.93;

Tpm = 20.4;
Tzco = 1.07;

    %% mijesce na wyslanie danych na serwer: sparsowanie 

    % stworzenie ramki do wyslania 
    % gotowe wysylanie danych dla kazdego
    %Data=sprintf('%+03.2f!%03.2f',To,Tzm); %E
    Data = sprintf('%+07.2f!%+3.2f',Tpm,Tzco) %W

    %Data=sprintf('%+03.2f',Tpco); %B
    %Data=sprintf('%+03.4f',Fzm); %R

    fopen(t);
    fwrite(t, [ID,'!SendData!', Data]);
    %fwrite(t, [ID,'!',Data]); niepotrzebne, dane u gory
    fclose(t);
        
    % ustalenie starego czasu
    time_old=time;
    day_old=day;
    
    

end
delete(t)
