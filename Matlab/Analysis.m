%% Bringing Data to Matlab
TX_samples = DataFromGRC('TX_samples','complex', 2^25);
RX_samples_1_channel =  DataFromGRC('RX_samples_1_channel', 'complex', 2^25);
RX_samples_2_channels_channel_A =  DataFromGRC('RX_samples_2_channels_channel_A', 'complex', 2^25);
RX_samples_2_channels_channel_B =  DataFromGRC('RX_samples_2_channels_channel_B', 'complex', 2^25);

%% Frequency analysis
N = length(RX_samples_1_channel);
fs = 8e6;
fprintf('Frequency Resoultion: %fHz\n', fs/N);
f = (-fs/2:fs/N:fs/2-fs/N)/1000;                          % frequency vector

figure(1)
mag = abs(fftshift(fft(TX_samples)));                     % magnitude spectrum
plot(f,mag); title('TX samples');
xlabel('f (kHz)'); ylabel('magnitude');

figure(2)
mag = abs(fftshift(fft(RX_samples_1_channel)));           % magnitude spectrum
plot(f,mag); title('RX Samples 1 channel');
xlabel('f (kHz)'); ylabel('magnitude');

figure(3)
mag = abs(fftshift(fft(RX_samples_2_channels_channel_A)));% magnitude spectrum
plot(f,mag); title('RX Samples 2 channels channel A');
xlabel('f (kHz)'); ylabel('magnitude');

figure(4)
mag = abs(fftshift(fft(RX_samples_2_channels_channel_A)));           % magnitude spectrum
plot(f,mag); title('RX samples 2 channels channel B');
xlabel('f (kHz)'); ylabel('magnitude');