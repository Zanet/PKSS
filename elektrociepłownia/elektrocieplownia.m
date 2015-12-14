%% init
clear all; %uwazajcie tu!

load('danestartowe.mat');
time=0;
flaga = 0;
time_old = 0;
day_old = 0;
%%
PORT_NUMBER = 8008;
IP = '192.168.43.48'; %ip serwera
ID = 'E' %identyfikator uzytkownika: E- elektrocieplowania, W- Wymiennik, R- Regulator, B- budynek
%nowa wersja matlaba
t = tcpip(IP, PORT_NUMBER, 'NetworkRole', 'client'); 


To = to(1);
Tzm = tzm(1);
Data=sprintf('%+07.2f!%+07.2f',To,Tzm); %E
fopen(t);
fwrite(t, [ID,'!SendData!', Data]);
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
        pause(0.2); % okres co jaki wysy³ana jest kolejna komenda keepalive
    end
    
    %% Odebranie czasu z serwera
   %while(time_old==time)
        fopen(t);
        fwrite(t, [ID,'!GetTime']);
        [A, COUNT] = fread(t, 9);
        if(COUNT > 0)
            text = char(A.')
        end
        fclose(t);
      pause(4);
 %  end
%Parsowanie danych o czasie otrzymanych z serwera:
    time=str2num(text(4:5))*3600+60*str2num(text(7:8))
    day=str2num(text(1:2))

    %% miejsce na odebranie danych z serwera i sparsowanie

    fopen(t);
    fwrite(t, [ID,'!GetParams']);
    disp('Odebrano parametry');
    %Tutaj uwaga: serwer na GetParams wysyla tekst o dlugosci
    % ilosc parametrow * DLUGOSC_REKORDU (10) + ilosc parametrow - 1
    %dla 3 parametrow mamy 32,
    %dla 2 parametrow mamy 21
    [A, COUNT] = fread(t, 10); % 3 parametry pobiera
    if(COUNT > 0)
        text = char(A.')
    end
    fclose(t);
    
    % przyk³adowe parsowanie odebranych danych
    %
    Tpm=str2num(text(2:4))+0.01*str2num(text(6:7))
    if(text(1)=='-')
        Tpm=-Tpm
    end
   % W
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
%     if(day_old == day || day_old == 0)
%     
%     else 
      if day_old ~= day
            flaga = 0;
            %zmienne ktore pobieramy z poprzedniego wywolania
            t0= tend; %zmienna ktora przypisuje do nowej poczatkowej wartosci statnia stara wartosc
            to_old =to;

            % Pierwsze Losowanie wspolczynnokow i wyliczanie przebiegu temperatury otoczenia w oknie 24h

            B= rand(3,1)*50; % 3 losowe liczby do wspo³czynników sinusa temperatury
            const1 = (B(1)+B(2))/2 + min(B); %const1 const2 wspo³czynniki sinusa
            const2 = (B(1)+B(2))/2 - min(B);

            probkowanie = -pi/2:2*pi/719:2*pi-pi/2; %równomierny podzia³ na 720 próbek (1 doba, probka co 2minuty)
            to = sqrt(const1*(14+6*sin (probkowanie)))-const2; %wzór wyliczaj¹cy kolejne wartoœci temperatury w ciagu calego dnia 



            % Ponowne Losowanie wspolczynnokow i wyliczanie przebiegu temperatury otoczenia w oknie 24h
            %Wywo³uje siê JE¯ELI wylosowana temperatura otoczenia z godziny 8.00
            %przekracza 10st

            while to(1) > 2 %zabezpieczenie, je¿eli temperatury dodatnie to losuj jesazcze raz
            B= rand(3,1)*50;   %losowe wybranie wspolczynnikow
            const1 = (B(1)+B(2))/2 + min(B);
            const2 = (B(1)+B(2))/2 - min(B);
            x = 0:2*pi/719:2*pi; %okno czasu - przyjalem 720probek przez 24h (probka na 2 min)
            %y = sqrt(40*(14+6*sin (x)))-30;
            y = sqrt(const1*(14+6*sin (x)))-const2; %NAJWAZNIEJSZE ROWNANIE - ROWNANIE WYLICZAJACE KOLEJNE TEMPERATURY oknie czasu
            to = y;
            end

            delta = (tend - to(1)); %
            podzial = delta:-delta/719:0;
            to = to + podzial;

            tend = to(720); %przypisanie ostatniej wartosci to do zmiennej tymczasowej
            to_old =[to_old to];

            for i= 1:1:720
               tzm(i) = 70 - 2.5*(to_old(485+i)-6);
            end
%             for i= 240:720
%                   tzm(i) = 70-2.5*(to(i-239)-6); %przesuniecie w czasie temperatury o 8h
%                   %tzm1(i) = to(i-239);
%              end
% 
%              for i = 482:720
%                 tzm(i-481) = 70-2.5*(to_old(i)-6) ; %dope³nienie pierwszych 8h symulacji
%                % tzm1(i-481)= to(i);
%              end

             for i = 1:720 %oba ograniszenia na temperature wody.
                if tzm(i) <70
                    tzm(i) = 70;
                elseif tzm(i) >135
                    tzm(i) = 135;
                end
             end
             %pierwsza probka tzm(1) i to(1) to stan na godzine 8.00 w jednostce AGH.

             % Graficzne przedstawienie wygenerowanych 

             figure(); %odglad wynikow
             plot(to,'b');
             hold on
             plot(tzm, 'r');
             legend ('to', 'tzm','Location',  'NorthOutside'  );



            figure();
            subplot(2,1,1)
            plot(to,'b');
            title('\bf Temperatura otoczenia (T_o) w przeci¹gu 24h');
            xlabel('\it czas [2min]');
            ylabel('\it temperatura [^oC]');
            subplot(2,1,2);
             plot(tzm, 'r');
            title('\bf Temperatura wystawionej wody (T_{zm}) w przeci¹gu 24h');
            xlabel('\it czas [2min]');
            ylabel('\it temperatura [^oC]'); %przejscie pomiedzy dniami
       % end
    end
%wywolanie Waszego modelu
    if (day == 2 && time>=61600)
       flaga = 1;
    end

%dane zwrocone z waszej symulacji powinny pojawic sie w workspace, tutaj
%dane do testow
if floor(time/120)==0
    z = 1;
else
    z = 0;
end

    To = to(floor(time/120)+z);
    if flaga == 1
        Tzm = Tpm;
    else
        Tzm = tzm(floor(time/120)+z);
    end
%Tpm = 20.4;
%Tzco = 1.07;

    %% mijesce na wyslanie danych na serwer: sparsowanie 

    % stworzenie ramki do wyslania 
    % gotowe wysylanie danych dla kazdego
    Data=sprintf('%+07.2f!%+07.2f',To,Tzm) %E
    %Data = sprintf('%+3.2f!%+3.2f',Tpm,Tzco) %W

    %Data=sprintf('%+03.2f',Tpco); %B
    %Data=sprintf('%+03.4f',Fzm); %R

    fopen(t);
    fwrite(t, [ID,'!SendData!', Data]);
    %fwrite(t, [ID,'!',Data]); niepotrzebne, dane u gory
    fclose(t);
    disp('Wyslano dane');
    % ustalenie starego czasu
    time_old=time;
    day_old=day;
    
    

end
delete(t)