function[filtered,t] = butterfilt_figure(filename)

matrix = load (filename);
x=matrix(:,2);%[deg]
t=matrix(:,1);%[msec]
t=t(1:(length(t)-1));
x1=diff(x);
t1=diff(t);
v=diff(x)/diff(t);
%plot(t,v);
%hold;
figure;
hold on;

% spectrum=(fft(v));
% Fs=500;
% T = 1/Fs;
% L = length(t);
% P2 = abs(spectrum/L);
% P1 = P2(1:L/2+1);
% P1(2:end-1) = 2*P1(2:end-1);
% f=Fs*(0:(L/2))/L;
%[a1 , b1]=butter(1,3/250);
%butterfilt(order,fc/(fs/2))
%in this case ord=4, fc=3, fs=500
% 
% filtered=filtfilt(a1,b1,v);
% plot(t,filtered);
% [a1 , b1]=butter(2,3/250);
% %butterfilt(order,fc/(fs/2))
% %in this case ord=4, fc=3, fs=500
% 
% filtered=filter(a1,b1,v);
% plot(t,filtered);
% [a1 , b1]=butter(3,3/250);
% %butterfilt(order,fc/(fs/2))
% %in this case ord=4, fc=3, fs=500

% filtered=filter(a1,b1,v);
% plot(t,filtered);
[a1 , b1]=butter(4,3/250);
%butterfilt(order,fc/(fs/2))
%in this case ord=4, fc=3, fs=500

filtered=filter(a1,b1,v);
plot(t,filtered);
% [a1 , b1]=butter(5,3/250);
% %butterfilt(order,fc/(fs/2))
% %in this case ord=4, fc=3, fs=500
% 
% 
% filtered=filter(a1,b1,v);
% plot(t,filtered);
end