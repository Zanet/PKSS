load('server_data.mat');

f=plot(Czas,T_o,Czas,T_pco,Czas,T_zco)
set(f,'LineWidth',2)
xl=xlabel('Czas [min]');
yl=ylabel('Temperatura [ ^o C]');
legend('T_o','T_{pco}','T_{zco}')
set(xl,'FontSize',14);
set(yl,'FontSize',14);
set(gca,'Fontsize',16);
axis([Czas(1) Czas(end) min([T_o; T_zco; T_pco])*0.9 max([T_o; T_zco; T_pco])*1.1 ]);
