function [maxTheory, maxFilter] = IdealvsActualPlots(vanillaFile, modifiedFile, targetFreq)

finx=fopen(vanillaFile,'rb');
finy=fopen(modifiedFile,'rb');
if(finy==-1 || finx==-1)fprintf( 'ERROR : Could not open file .\n '); end
[headery,~]=fread(finy,5,'int');
[headerx,~]=fread(finx,5,'int');
fsx=headerx(4);
fsy=headery(4);
if(fsy==0) fsy=1; end
nxsamples=headerx(3);
nysamples=headery(3);

x=zeros(nxsamples,1,'double');
y=zeros(nysamples,1,'double');
for i=1:nxsamples
    [x(i),~]=fread(finx,1,'float');
end
for i=1:nysamples
    [y(i),~]=fread(finy,1,'float');
end

close all;
nfft=2^15;
freqx=([0:nfft-1]/nfft-0.5)*fsx;
freqy=([0:nfft-1]/nfft-0.5)*fsy;
X=fftshift(fft(x,nfft));
Y=fftshift(fft(y,nfft));
[H, ~] = IdealLPF(300,400,11025,nfft);
X=X.*H.';


figure(1);
subplot(1,2,1);
plot(freqx,20*log10(abs(X)));
maxTheory = max(20*log10(abs(X)));
xlim([-2*targetFreq 2*targetFreq]);
ylim([-60 80]);
xlabel('Frequency [Hz]','FontSize',12);
ylabel('Magnitude [dB]','FontSize',12);
title('Theoretical Filtered Magnitude','FontSize',8);
set(gca,'FontSize',10);
grid on;

%figure('name','Y magnitude');
subplot(1,2,2);
plot(freqy,20*log10(abs(Y)));
maxFilter = max(20*log10(abs(Y)));
xlim([-2*targetFreq 2*targetFreq]);
ylim([-60 80]);
xlabel('Frequency [Hz]','FontSize',12);
ylabel('Magnitude [dB]','FontSize',12);
title('c++ Fast Convolution Magnitude','FontSize',8);
set(gca,'FontSize',10);
grid on;
%legend('X(w)','Y(w)');

end