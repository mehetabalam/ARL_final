
function[x] = compute_filter(filename)
matrix = load (filename);
x=matrix(:,2);
t=matrix(:,1);
t=t(1:(length(t)-1));
v=diff(x)/4;
spectrum=(fft(v));
Fs=250;
T = 1/Fs;
L = length(t);
P2 = abs(spectrum/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f=Fs*(0:(L/2))/L;
plot(f,P1); %we got the single sided spectrum
figure;
%we now get the minimums(3 of the spectrum and we use a butterworth filter of different order for everything)
%34.48 %55.17 %68.96
%plot(t,v);
hold;


% n=3;
% %parameters for the filter 6 hz cut and 5 order filter
% while n <7
% [a1 b1]=butter(4,n/125);
% opt=filter(a1,b1,v);
% plot(t,opt);
% %env (9-n) = max (opt);
% %tt(9-n) = find(opt==max(opt));
% %err (n) = mean((opt - v).^2);
% n=n+1;
% %err
% end


%denv=env(2:8)-env(1:7);
%dtt=tt(2:8)-tt(1:7);
%ddtt=dtt(2:7)-dtt(1:6);
%ddenv=denv(2:7)-denv(1:6);
%dd(1:6)=ddenv(1:6)./ddtt(1:6)



n=3;
while n < 7
[a1 b1]=butter(n,5/125);
opt=filter(a1,b1,v);
plot(t/1000,opt);
%env (9-n) = max (opt);
%tt(9-n) = find(opt==max(opt));
%err (n) = mean((opt - v).^2);
n=n+1;
%err
end

%x=dd;


end