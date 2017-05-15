% transfer function to control
% 1/(s+3)(s+7)
%
% numP=[1];
% denP=conv([1 3],[1 7]);
% P=tf(numP,denP);
% 
% dur=20;
% step(P,dur);

%controller

k=92;
numC = [-24.6 -15.7];
denC = [0 -1800];

%zeroes=[-0.5 ]';
%poles=[0 -1+5i -1-5i]
%[numC,denC]=zp2tf(zeroes,poles,k);
C=tf(numC,denC);


%combined open loop sys

%G=P*C;

%draw root locus

figure;
rlocus(G*C);
sgrid;

ctrlf=G*k*C/(1+G*k*C);
figure;
step(ctrlf,dur);
ctrlparams=stepinfo(ctrlf,'RiseTimeLimits',[0.05,0.95],'SettlingTimeThreshold',0.05);
[wn,zeta,poles] = damp(tf(G*k*C,1+G*k*C));
%divergence condition
if  real(rlocus(G,k))<=0
    disp('not divergent');
end
%settling time condition
if  ctrlparams.SettlingTime<=0.5
    disp('settling time ok');
end
%overshoot condition
if  ctrlparams.Overshoot<10
    disp('overshoot ok');
end
%steady state error condition
if  abs((1-ctrlparams.SettlingMax)/100)<1
    disp('steady state error ok');
end