clear all;
load('server_data.mat');
Czas=24*60*day+60*hour+minute;

subplot(2,1,1);
f=plot(Czas,T_o,Czas,T_pco,Czas,T_zco,Czas, T_bud,Czas, T_pm,Czas, T_zm )
set(f,'LineWidth',2)
xl=xlabel('Czas [min]');
yl=ylabel('Temperatura [ ^o C]');
legend('T_o','T_{pco}','T_{zco}','T_{bud}','T_{pm}','T_zm')
set(xl,'FontSize',14);
set(yl,'FontSize',14);
set(gca,'Fontsize',16);
axis([Czas(1) Czas(end)+100 min([T_o; T_zco; T_pco])*1.5 max([T_o; T_zco; T_pco])*1.1 ]);
subplot(2,1,2);
f=plot(Czas,F_zm)
axis([Czas(1) Czas(end)+100 min(F_zm)*1.5 max(F_zm)*1.1 ]);
set(f,'LineWidth',2)
xl=xlabel('Czas [min]');
yl=ylabel('Temperatura [ ^o C]');
legend('Sterowanie F_zm')
set(xl,'FontSize',14);
set(yl,'FontSize',14);
set(gca,'Fontsize',16);
saveas(gcf,'Wykres1.png');