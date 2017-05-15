function[filtered,t] = butterfilt(filename)

matrix = load (filename);
x=matrix(:,2);%[deg]
t=matrix(:,1);%[msec]
t=t(1:(length(t)-1));
v=diff(x)/4;
%plot(t,v);
%hold;
% spectrum=(fft(v));
% Fs=500;
% T = 1/Fs;
% L = length(t);
% P2 = abs(spectrum/L);
% P1 = P2(1:L/2+1);
% P1(2:end-1) = 2*P1(2:end-1);
% f=Fs*(0:(L/2))/L;
[a1, b1]=butter(4,5/125);
filtered=(a1,b1,v);
%plot(t,filtered);
end