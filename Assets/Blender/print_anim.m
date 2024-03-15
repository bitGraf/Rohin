%% create plots
figure(1);
clf;
grid on;
hold on;
xlabel('Time (s)');
title('Translation');
legend on;

figure(2);
clf;
grid on;
hold on;
xlabel('Time (s)');
title('Rotation');
legend on;

% figure(3);
% clf;
% grid on;
% hold on;
% xlabel('Time (s)');
% title('Scale');
% legend on;

%%
filename = '../../Game/run_tree/Data/Animations/anim.anim';
fid = fopen(filename, 'rb');

%% Header
MAGIC = char(fread(fid, 4, 'uchar'))';
filesize = fread(fid, 1, 'uint32');
version = fread(fid, 1, 'uint32');
flag = fread(fid, 1, 'uint32');
timestamp = fread(fid, 1, 'uint64');
num_bones = fread(fid, 1, 'uint16');
num_samples = fread(fid, 1, 'uint16');
frame_rate = fread(fid, 1, 'float32');

time = linspace(0, (num_samples-1)/frame_rate, num_samples);

%% Skeleton
MAGIC = char(fread(fid, 4, 'uchar'))';
if (~strcmp(MAGIC, 'SKEL'))
    fclose(fid);
    return;
end
if (fread(fid, 1, 'uint16') ~= num_bones)
    fprintf('Bone number mismatch!\n');
end
for n = 1:num_bones
    bone_idx = fread(fid, 1, 'uint16');
    parent_idx = fread(fid, 1, 'int16');
end

%% Bones
show = [3];
for n = 1:num_bones
    MAGIC = char(fread(fid, 4, 'uchar'))';
    if (~strcmp(MAGIC, 'BONE'))
        fprintf('missing BONE header!\n');
        break;
    end
    
    translation = reshape(fread(fid, 3*num_samples, 'float32'), 3, num_samples)';
    rotation    = reshape(fread(fid, 4*num_samples, 'float32'), 4, num_samples)';
    scale       = reshape(fread(fid, 3*num_samples, 'float32'), 3, num_samples)';
    
    if intersect(show, n)
        figure(1);
        plot(time, translation(:,1), 'DisplayName', sprintf('Bone %.0f X', n));
        plot(time, translation(:,2), '--', 'DisplayName', sprintf('Bone %.0f Y', n));
        plot(time, translation(:,3), '.', 'DisplayName', sprintf('Bone %.0f Z', n));
% 
%         figure(2);
%         plot(time, rotation(:,1), 'DisplayName', sprintf('Bone %.0f X', n));
%         plot(time, rotation(:,2), 'DisplayName', sprintf('Bone %.0f Y', n));
%         plot(time, rotation(:,3), 'DisplayName', sprintf('Bone %.0f Z', n));
%         plot(time, rotation(:,4), 'DisplayName', sprintf('Bone %.0f W', n));

    %     figure(3);
    %     plot(time, scale(:,1), 'DisplayName', sprintf('Bone %.0f X', n));
    %     plot(time, scale(:,2), 'DisplayName', sprintf('Bone %.0f Y', n));
    %     plot(time, scale(:,3), 'DisplayName', sprintf('Bone %.0f Z', n));
    end
end

fclose(fid);