clc;
clear all;
close all;

%--------------------------------------------------------------------------
%load images
% im_dest = im2double(imread('left.png')); 
% im_obj  = im2double(imread('right.png')); 
% mask0   = im2double(imread('mask.png'));

%im_dest = im2double(imread('target_m.png')); 
%im_obj  = im2double(imread('sourcescarlet.png')); 
%mask0   = im2double(imread('mask2.png'));

%im_dest = im2double(imread('C:\Users\Henz\Dropbox\MVC\MVC\dtarget22.bmp')); 
%im_obj  = im2double(imread('C:\Users\Henz\Dropbox\MVC\MVC\Urso\source.bmp')); 
%mask0   = im2double(imread('C:\Users\Henz\Dropbox\MVC\MVC\Urso\mask.bmp'));

im_dest = im2double(imread('target_0.png')); 
im_obj  = im2double(imread('source_0.png')); 
mask0   = im2double(imread('mask_0.png'));

%im_dest = im2double(imread('hand_00.png')); 
%im_obj  = im2double(imread('eye_00.png')); 
%mask0   = im2double(imread('mask_00.png'));

mask = mask0(:,:,1);

[R C CH] = size(im_dest);

%--------------------------------------------------------------------------
%simple merge

im_seam  = zeros(R, C, CH);

for k = 1:CH
    im_seam(:,:,k) = mask .* im_obj(:,:,k) + (1-mask) .* im_dest(:,:,k);
end;

%--------------------------------------------------------------------------
% PoissonSolver: see "poissonSolver.m" file for details
%  
% parameters 
% imSrc  - source image 
% imDest - destination image 
% imMask - a black and white mask to indicate the irregular boundary 
% Opaque 1=copy opaque, 0= transparent
for k = 1:CH
    im_poisson1(:,:,k) = poissonSolver(im_obj(:,:,k), im_dest(:,:,k), mask, 1);
    %im_poisson2(:,:,k) = poissonSolver(im_obj(:,:,k), im_dest(:,:,k), mask, 0);
end;
%--------------------------------------------------------------------------


figure; 
subplot(1,3,1); imshow(im_obj); 
subplot(1,3,2); imshow(im_dest);  
subplot(1,3,3); imshow(mask); 
figure;
subplot(1,2,1);imshow(im_seam);
subplot(1,2,2);imshow(im_poisson1);
figure;
imshow(im_poisson1);
imsave();
%subplot(2,3,6); imshow(im_poisson2); 

%figure; 
%subplot(1,2,1); imshow(im_poisson1); 
%subplot(1,2,2); imshow(im_poisson2); 
