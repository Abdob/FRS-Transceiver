%% single_RX_1 data, device arg bladerf=0 
single_RX_1 = DataFromGRC('single_RX_1', 'complex');
Nsamp = 8*40*2;                           % get little bit of samples
single_RX_1 = single_RX_1(end-Nsamp:end); % get last samples recorded
plot(real(single_RX_1)); grid on; xlabel("sample"); title("single RX 1 data; bladerf=0 ");

%% dual_RX_1 data, device arg bladerf=0,nchan=2 
dual_RX_1 = DataFromGRC('dual_RX_1', 'complex');
Nsamp = 8*40*2;                           % get little bit of samples
dual_RX_1 = dual_RX_1(end-Nsamp:end); % get last samples recorded
plot(real(dual_RX_1)); grid on; xlabel("sample"); title("dual RX 1 data; bladerf=0,nchan=2");


