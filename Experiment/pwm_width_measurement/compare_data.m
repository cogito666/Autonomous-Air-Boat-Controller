clear all;
% pulse1 = 750; pulse2 = 2250;
% pulse1 = 1000; pulse2 = 2250;
pulse1 = 1234; pulse2 = 2250;
% pulse1 = 1500; pulse2 = 2250;

polling_data = load(strcat('polling_MEGA2560_',num2str(pulse1), '_', num2str(2250),'.txt'));
interrupt_data = load(strcat('interrupt_MEGA2560_',num2str(pulse1), '_', num2str(2250),'.txt'));

figure(1); clf
subplot(2,1,1);hold on; grid on; title('Measurements(\musec)','FontSize',15);
plot(polling_data(:,1), 'b'); 
plot(interrupt_data(:,1),'r');
legend('polling 750\musec','interrupt 750\musec')
subplot(2,1,2);hold on; grid on;
plot(polling_data(:,2), 'b'); 
plot(interrupt_data(:,2),'r');
legend('polling 2250\musec','interrupt 2250\musec')

figure(2); clf
subplot(2,1,1);hold on; grid on; title('Measurement Error(\musec)','FontSize',15);
plot(pulse1-polling_data(:,1), 'b'); 
plot(pulse1-interrupt_data(:,1),'r');
legend('polling 750\musec','interrupt 750\musec');
subplot(2,1,2);hold on; grid on;
plot(pulse2-polling_data(:,2), 'b'); 
plot(pulse2-interrupt_data(:,2),'r');
legend('polling 2250\musec','interrupt 2250\musec')