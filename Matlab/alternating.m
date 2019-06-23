%% Generate Square-root raised cosine pulse

beta = 0.5; % roll off factor for pulse shape
span = 6;   % how many pulse periods each spans
sps = 8;    % oversampling factor, samples per pulse

B = rcosdesign(beta, span, sps);
plot(B); grid on; xlabel('sample'); title('Square-root raised cosine pulse');


%% Generate a sequence of alternating SRRC pulses
pattern = cos((1:60)*pi); % alternating pattern
pattern = upsample(pattern, sps); % upsample to meet pulse sampling rate
sequence = filter(B, 1, pattern); % pulse shaping pattern into SRRC sequence
figure(2); plot(sequence); grid on; xlabel('sample');title('SRRC alternating sequence');

%% Store the generated sequence in data file for GRC
sequence = sequence + j*sequence;    % duplicate real to imaginary part, for complex samples 
DataToGRC('alternating', sequence, 'complex');