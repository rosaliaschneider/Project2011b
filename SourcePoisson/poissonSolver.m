function [imNew] = poissonSolver(imSrc, imTarget, imMask, Opaque) 
% 
% parameters 
% imSrc  - source image 
% imTarget - target image 
% imMask - binary mask representing the region 
% Guidance Field Mode - 1 = Source Gradient, 0 = Mixed Gradient
% create the matrix and vector for the linear system
% M.X = b
% and solve it (find x)

[R C] = size(imSrc); 
                            
%--------------------------------------------- 
% sparse matrix allocation 
%--------------------------------------------- 

%number of pixels to copy from object, 
n = size(find(imMask), 1); %number of pixels inside the region
M = spalloc(n, n, 5*n); 
fprintf('\nNumber of pixels in region: %d\n',n);
%vector for the boundary condition 
b = zeros(1, n);

%  use index to create a vector (the vector X of the equation M.X = b) of
%  mask pixels
% imIndex: matrix index holder, given a (x,y) return the index in the
% vector X

imIndex = zeros(R, C); 

index = 0; 
for row = 1:R 
    for col = 1:C 
        if imMask(row, col) ~= 0 
            index = index + 1;             
            imIndex(row, col) = index; 
        end 
    end 
end 
         

%--------------------------------------------- 
% construct the matrix M 
%--------------------------------------------- 

row = 0;  
for y = 1:R 
    for x = 1:C 
 
        if imMask(y, x) ~= 0 
 
            row = row + 1;    

            M(row, row) = -4; 
                    
            % if north pixel is out the region
            if imMask(y-1, x) ~= 0 
                col = imIndex(y-1, x); 
                M(row, col) = 1; 
            end 
             
            % if west pixel is out the region
            if imMask(y, x-1) ~= 0 
                col = imIndex(y, x-1); 
                M(row, col) = 1; 
            end             
             
            % if south pixel is out the region
            if imMask(y+1, x) ~= 0 
                col = imIndex(y+1, x); 
                M(row, col) = 1; 
            end 
             
            % if east pixel is out the region
            if imMask(y, x+1) ~= 0 
                col = imIndex(y, x+1); 
                M(row, col) = 1; 
            end        
              
        end 
    end 
end 

%--------------------------------------------- 
% construct the vector b 
%--------------------------------------------- 

row = 0;  
for y = 1:R 
    for x = 1:C 
 
        if imMask(y, x) ~= 0 
 
            row = row + 1;    
             
            % Dirichlet condition: 
            % pixel values on the boundary match with target image
            
            % if north pixel is out the region 
            if imMask(y-1, x) == 0 
                b(row) = b(row) - imTarget(y-1, x); 
            end 
             
            % if west pixel is out the region
            if imMask(y, x-1) == 0 
                b(row) = b(row) - imTarget(y, x-1); 
            end             
             
            % if south pixel is out the region
            if imMask(y+1, x) == 0 
                b(row) = b(row) - imTarget(y+1, x); 
            end 
             
            % if east pixel is out the region
            if imMask(y, x+1) == 0 
                b(row) = b(row) - imTarget(y, x+1); 
            end        
                         
            % construct the guidance vector field v	 
            
            % source gradient      
            vs1 = imSrc(y, x) - imSrc(y-1, x); 
            vs2 = imSrc(y, x) - imSrc(y+1, x); 
            vs3 = imSrc(y, x) - imSrc(y, x-1); 
            vs4 = imSrc(y, x) - imSrc(y, x+1); 
    
            if Opaque  %opaque case: use object gradient
                v =  -(vs1 + vs2 + vs3 + vs4);            
            else  %mix gradients of object and dest
                
                %dest gradient (only for transparent case)
                vt1 = imTarget(y, x) - imTarget(y-1, x); 
                vt2 = imTarget(y, x) - imTarget(y+1, x); 
                vt3 = imTarget(y, x) - imTarget(y, x-1); 
                vt4 = imTarget(y, x) - imTarget(y, x+1); 
                
                %select the stronger components
                if abs(vs1) > abs(vt1)  vmax1 = vs1;  else vmax1 = vt1; end 
                if abs(vs2) > abs(vt2)  vmax2 = vs2;  else vmax2 = vt2; end 
                if abs(vs3) > abs(vt3)  vmax3 = vs3;  else vmax3 = vt3; end 
                if abs(vs4) > abs(vt4)  vmax4 = vs4;  else vmax4 = vt4; end 
                
                v =  -(vmax1 + vmax2 + vmax3 + vmax4);
            end;    
            
            b(row) = b(row) + v; 
        end 
    end 
end 
   
% use bi-conjugate gradient method to solve the system
X = bicg(M, b', [], 300); %[] is the default tolerance
 
%--------------------------------------------- 
% construct the final composition
%--------------------------------------------- 
imNew = imTarget; 
 
fprintf('\nfilling destination image\n'); 
% now fill in the  
for y = 1:R 
    for x = 1:C 
        if imMask(y, x) ~= 0 
            index = imIndex(y, x); 
            imNew(y, x) = X(index); 
        end 
    end 
end 
  