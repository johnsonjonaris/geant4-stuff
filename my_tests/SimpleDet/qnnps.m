%% load images
folder = 'D:\Workspace\results\';
files = dir(folder);
nFiles = length(files);
nx = 160; ny = 160;
images = zeros(nx,ny,nFiles-2,'uint8');
c = 1;
for i = 3:nFiles
    filename = [folder, files(i).name];
    fileId = fopen(filename,'r');
    image = fread(fileId, [nx,ny], 'uint8');
    fclose(fileId);
    s = sum(image(:));
    if ( s > 0)        
        images(:,:,c) = image;
        c = c + 1;
    end
end
c = c-1;
images = images(:,:,1:c);
images(1,1,:) = 0;
% displayProjection(images)
%% compute NPS
PSF_X = squeeze(sum(images,1))';
nfft = 512;
NPS = abs(fft(PSF_X,nfft,2)).^2;
dx = 1.016 + 0.05; % mm
NPS = NPS(:,1:nfft/2);
NPS_avg = mean(NPS)';
NPS_avg_nor = NPS_avg./NPS_avg(1);
%% compute scaling factor
T = sum(PSF_X,2);
[n,xout] = hist(T,100);
% figure;
% stem(xout,n)
pdf = n./sum(n);
% computing moments (not central moments = not around the mean)
M0 = 1;
M1 = sum(xout.*pdf);
M2 = sum(((xout).^2).*pdf);
Ngamma = nFiles-2;
Np = c;
eta = Np/Ngamma;
qNNPS0 = M2*M0/(eta*M1^2);
%% compute qNNPS
qNNPS = qNNPS0*NPS_avg_nor;
figure;
FS = 1/dx; % sampling frequency
sf = (0:nfft/2-1)*FS/nfft; % spatial frequency (1/mm)
plot(sf,qNNPS)
xlabel('spatial frequency 1/mm');
ylabel('qNNPS(f)');
