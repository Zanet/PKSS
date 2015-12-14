%% init
clear all; %uwazajcie tu!

time=0;
time_old = 0;
day_old = 0;



open('wymiennik.slx');
mdl = bdroot;

PORT_NUMBER = 8008;
IP = '192.168.43.48'; %ip serwera
ID = 'B' %identyfikator uzytkownika: E- elektrocieplowania, W- Wymiennik, R- Regulator, B- budynek
%nowa wersja matlaba

t = tcpip(IP, PORT_NUMBER,'NetworkRole', 'client');
 
    Data=sprintf('%+07.02f!%+09.04f!%+07.02f',50.0,40.0,15.0); %B
    fopen(t);
    fwrite(t, [ID,'!SendData!', Data]);
    %fwrite(t, [ID,'!',Data]); niepotrzebne, dane u gory
    fclose(t);
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
        pause(2); % okres co jaki wysy³ana jest kolejna komenda keepalive
    end
    
    %% Odebranie czasu z serwera
    fopen(t);
    fwrite(t, [ID,'!GetTime']);
    [A, COUNT] = fread(t, 9);
    if(COUNT > 0)
        text = char(A.');
        
    end
    fclose(t);

%Parsowanie danych o czasie otrzymanych z serwera:
    day=str2num(text(1:2))
    time_old
    time=str2num(text(4:5))*3600+60*str2num(text(7:8))+86400*day
    
    godzina=str2num(text(4:5))
    %% miejsce na odebranie danych z serwera i sparsowanie

    fopen(t);
    fwrite(t, [ID,'!GetParams']);
    
    %Tutaj uwaga: serwer na GetParams wysyla tekst o dlugosci
    % ilosc parametrow * DLUGOSC_REKORDU (10) + ilosc parametrow - 1
    %dla 3 parametrow mamy 32,
    %dla 2 parametrow mamy 21
    [A, COUNT] = fread(t, 32); % 3 parametry pobiera
    if(COUNT > 0)
        text = char(A.');
    end
    fclose(t);
    
    %  parsowanie odebranych danych
    %B
    Tzco=str2num(text(2:4))+0.01*str2num(text(6:7))
     if(text(1)=='-')
         Tzco=-Tzco;
     end
     To=str2num(text(10:12))+0.01*str2num(text(14:15))
     if(text(9)=='-')
         To=-To;
     end
     
     
     Kp=str2num(text(17:18))+0.01*str2num(text(20:21))
     Ti=str2num(text(23:24))+0.01*str2num(text(26:27))
    % Kp=str2num(text(16:17));
     %Ti=str2num(text(19:20));

     
     
    %komenda str2num zamienia licze zapisana w string na liczbe
    
    %% miejsce na uruchomienie symulacji
    if(day_old == day || day_old == 0)
        sim_time = time-time_old;
    else if day_old ~= day
            sim_time=time+86400-time_old; %przejscie pomiedzy dniami
        end
    end

%wywolanie Waszego modelu
if( time_old~=time)
    zm=1
    set_param(mdl, 'SaveFinalState', 'on', 'FinalStateName',...
     [mdl 'SimState'],'SaveCompleteFinalSimState','on')
  
    [t1, Y1] = sim(mdl, [time_old time]);
end
    Tr
    %sim('model',time-time_old);
    
     %Data=sprintf('%+07.02f!%+07.02f!%+07.02f!%+07.00f',Tpco,Fzco,Tr,Tzad)
    Data=sprintf('%+07.02f!%+09.04f!%+07.02f',Tpco,Fzco,Tr) %B
    fopen(t);
    fwrite(t, [ID,'!SendData!', Data])
    %fwrite(t, [ID,'!',Data]); niepotrzebne, dane u gory
    fclose(t);



    %% mijesce na wyslanie danych na serwer: sparsowanie 



        
    % ustalenie starego czasu
    time_old=time;
    day_old=day;
    
    

end
delete(t)
