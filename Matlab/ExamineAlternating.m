%% GRC single_RX_1 data, device arg bladerf=0
figure(1)
single_RX_1 = DataFromGRC('single_RX_1', 'complex');
Nsamp = 4096;                           % get little bit of samples
single_RX_1 = single_RX_1(end-Nsamp:end); % get last samples recorded
plot(real(single_RX_1)); grid on; xlabel("sample"); title("single RX 1 data; bladerf=0,agc=1");

%% GRC dual_RX_1 data, device arg bladerf=0,nchan=2 
figure(2)
dual_RX_1 = DataFromGRC('dual_RX_1', 'complex');
Nsamp = 4096;                           % get little bit of samples
dual_RX_1 = dual_RX_1(end-Nsamp:end); % get last samples recorded
plot(real(dual_RX_1)); grid on; xlabel("sample"); title("GRC Channel 1: bladerf=0,nchan=2");
figure(3)
dual_RX_2 = DataFromGRC('dual_RX_2', 'complex');
Nsamp = 4096;                           % get little bit of samples
dual_RX_2 = dual_RX_2(end-Nsamp:end); % get last samples recorded
plot(real(dual_RX_2)); grid on; xlabel("sample"); title("GRC Channel 2: bladerf=0,nchan=2");

%% BladeRF-CLI single - First import single.csv
figure(4)
single1 = single{:,1};
single2 = single{:,2};
plot(single1);
title('rx config file=samples.csv format=csv n=4k')

%%  BladeRF-CLI dual - First import dual.csv
figure(5)
dual1 = dual{:,1};
dual2 = dual{:,2};
plot(dual1);
title('Channel 1: rx config file=dual.csv format=csv n=8K channel=1,2');

figure(6)
dual3 = dual{:,3};
dual4 = dual{:,4};
plot(dual4);
title('Channel 2: rx config file=dual.csv format=csv n=4K channel=1,2');



