
function[wn,q,zeta,xnew] = compute_wqe(signal,t,power)
ln = @log;
%extract overshoot and time constants
x=stepinfo(signal,t,'SettlingTimeThreshold',0.05);

xnew = x;

%needed for computation: power (input), overshoot percentage, settling time
%steady state value


%settling time (conversion from msec to sec)
settling_time=x.SettlingTime/1000;

%steady state
%using a polinomial fitting to find steady state, consider signal only
%after settling time (e.g. 500ms)
steady_state=polyfit(t(125:end),signal(125:end),0);

%overshoot
overshoot=x.Overshoot;

%compute q,wn,zeta
q=100*steady_state/power;
zeta= sqrt(((ln(overshoot/100))^2)/(pi^2+((ln(overshoot/100))^2)));
%wn=-ln(0.03)/(zeta*settling_time)
wn=(ln(0.05)-ln(1/sqrt(1-zeta^2)))/(-(zeta*settling_time));
%compute transfer function
% num=q;
% den=[1/(wn^2) 2*zeta/wn 1];
% sys= tf(num,den);


% %assignin('base', 'sys100',sys100);
% 
%  a=step(sys,t);
%  ainfo= stepinfo(a);
%  %[y,t] = step(sys,Tfinal)
%  [b,tt]=step(sys*(x.SettlingMax+x.SettlingMin)/(ainfo.SettlingMax+ainfo.SettlingMin),30000);
%  %(x.SettlingMax+x.SettlingMin)/2
% figure
% hold;
% %length(b)
% 
% var = (x.SettlingMax+x.SettlingMin)/(ainfo.SettlingMax+ainfo.SettlingMin);
% 
% 
% plot (tt,b);
% plot(t,filt_v);
% assignin('base','s',size(filt_v))

end