global x1 x2;

basic = 'D:\Research\_ Project2011b\Data\Recitation13\';

fileH = fopen(strcat(basic,'_LocalHoms.txt'));
numFrames = fscanf(fileH, '%d', 1);

fileH2 = fopen(strcat(basic,'_NewLocalHoms.txt'), 'w');
fprintf(fileH2, '%d\n', numFrames);

%read first identity - not necessary
H = zeros(3, 3);
for i = 1:3
    for j = 1:3
        a = fscanf(fileH, '%lg', 1);
        H(i, j) = a(1);
    end
end
for i = 1:3
    for j = 1:3
        a = fprintf(fileH2, '%lg', H(i, j));
    end
end

for frame = 1:100

    H = zeros(3, 3);
    for i = 1:3
        for j = 1:3
            a = fscanf(fileH, '%lg', 1);
            H(i, j) = a(1);
        end
    end

    frame
    H

    fileX = fopen(strcat(basic,'Inliers\state_',num2str(frame-1),'.txt'));

    len1 = fscanf(fileX, '%d', 1);
    for i = 1:len1
        a = fscanf(fileX, '%lg', 1);
        x1(i, 1) = a(1);
        a = fscanf(fileX, '%lg', 1);
        x2(i, 1) = a(1);
        a = fscanf(fileX, '%lg', 1);
        x1(i, 2) = a(1);
        a = fscanf(fileX, '%lg', 1);
        x2(i, 2) = a(1);
        x1(i, 3) = 1;
        x2(i, 3) = 1;
    end

    H = lsqnonlin(@f, H);
    H
    
    for i = 1:3
        for j = 1:3
            a = fprintf(fileH2, '%lg', H(i, j));
        end
    end
    
    
end

fclose(fileH2)