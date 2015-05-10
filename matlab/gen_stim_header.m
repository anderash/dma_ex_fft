clear all;

%% Parameters
L = 2^10; % 1K
n = 0:1/L:1;

%% Compute Cosine LUT
stim_lut = 0 + (cos(100*pi*n)+cos(2*10000*pi*n))*2^13;
disp(['max(stim_lut)=' num2str(max(stim_lut)) ', min(stim_lut)=' num2str(min(stim_lut))]);

%% Plot Cosine LUT
figure(1);

subplot(2,1,1);
plot(stim_lut);

subplot(2,1,2);
plot(abs(fftshift(fft(stim_lut, L))));

%% Create header
fid = fopen('../sw/stim.h', 'w');
fprintf(fid, 'int stim_buf[MAX_FFT_LENGTH] = \n');
fprintf(fid, '{\n');

%% Write data to file
for ii = 1:L-1
    fprintf(fid, '    %d, ', round(stim_lut(ii)));
    if (mod(ii, 16) == 0)
        fprintf(fid, '\n');
    end
end
fprintf(fid, '    %d\n', round(stim_lut(ii)));
fprintf(fid, '};\n');

fclose(fid);
