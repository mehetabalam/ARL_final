%call every function from this script
files = dir('*.txt');

index=1;
clear('filtered_signals','models','itae','itse');
hide_plot=0;
%legend('training data','computed model','test data','model for test data');
if hide_plot==0
    figure;
    hold on;
    subplot(1,2,1);
    hold(subplot(1,2,1), 'on');
    title('Model Verification');
end

for file = files'
    
    %get power percentage from namefile
    power = sscanf(file.name, 'data%d.txt');
    
    %training set
    if mod(power,10)==0;
        
        %filter the signals from data
        [filtered_signals{index},t]=butterfilt(file.name);
          
        if hide_plot==0
            %plot training data
            tr_data=plot(t/1000,filtered_signals{index},'r');
        end
        
        %compute model parameters for each signal, create a model for each
        %signal
        [wn(index),q(index),eps(index),xnew(index)]=compute_wqe(filtered_signals{index},t,power);
        
        index=index+1;
        
    end
    
end


%compute filtered mean of parameters
avgwn=trimmean(wn,20);
avgq=trimmean(q,20);
%pp=[10 20 30 40 50 60 70 80 90 100];
%avgq = polyfit(pp(1:10),q(1:10),2);
avgeps=trimmean(eps,20);

for index = 1:10
    
    %compute model for training data
    %training data power goes form 10 to 100 (step 10), power=index*10/100
    power=index/10;
    num=avgq*power;
    %num = power*polyval(avgq,power*100);
    den=[1/(avgwn^2) 2*avgeps/avgwn 1];
    G= tf(num,den);
    
    if hide_plot==0
        %plot computed models
        step_response=step(G,t/1000,'g');
        tr_mod=plot(t/1000,step_response,'g');
    end
    
end

if hide_plot==0
    legend([tr_data,tr_mod],{'training data','model output'})
    xlabel('Time(ms)') % x-axis label
    ylabel('Angular Velocity(degree/msec)') % y-axis label
    subplot(1,2,2);
    hold(subplot(1,2,2), 'on');
    title('Model Validation');
    xlabel('Time(ms)') % x-axis label
    ylabel('Angular Velocity(degree/msec)') % y-axis label
end

index=1;
for file = files'
    
    %get power percentage from namefile
    power = sscanf(file.name, 'data%d.txt');
    
    %test set
    if mod(power,10)==5;
  
        %now compare model against test data and compute rms
        %test data power goes from 15 to 95 (step 10)
        
        [filtered_test_signals{index},t]=butterfilt(file.name);
        
        if hide_plot==0
            %plot test data
            test_data=plot(t/1000,filtered_test_signals{index},'m');
        end
        
        power=power/100;
        num=avgq*power;
    %num = (power/100) * polyval(avgq,power);
        den=[1/(avgwn^2) 2*avgeps/avgwn 1];
        G=tf(num,den);
        models{index}=step(G,t/1000);
         
        if hide_plot==0
            %plot test models
            test_mod=plot(t/1000,models{index},'b');
            legend([test_data,test_mod],{'test data','simulated output'});
        end
        
        error{index}=(models{index}-filtered_test_signals{index});
        %compute rmse
        rmse(index)=sqrt(sum(error{index}.^2)/numel(models{index}));
        %compute imse
        imse(index)=sum(error{index}.^2)/numel(models{index});
        %compute ise
        ise(index)=sum(error{index}.^2);
        % compute iae
        iae(index)=sum(abs(error{index}));
        %compute itse
        itse{index}=sum((t/1000)*(error{index}.^2)');
        % compute itae
        itae{index}=sum((t/1000)*(abs(models{index}-filtered_test_signals{index}))');
       
        index=index+1;
   
    end
        
end
    

        num=avgq;
        den=[1/(avgwn^2) 2*avgeps/avgwn 1];
        G=tf(num,den);


%%%here ends main code

% k=16.412;
% zeroes = [-14.5 -17.5];
% poles = [0 -123];

% k=0.4405;
% zeroes = [-32.92];
% poles = [0];

% k=0.37598;
% zeroes = [-69.04 -63.71];
% poles = [0 -103];

% k=0.41462;
% zeroes = [-34.41];
% poles = [0];

k=0.21763;
zeroes = [-35.99];
poles = [0];

C=zpk(zeroes,poles,k);
Cz = c2d(C,0.004,'tustin');
[zp,zz]=pzmap(Cz);
%C=tf(numC,denC);
