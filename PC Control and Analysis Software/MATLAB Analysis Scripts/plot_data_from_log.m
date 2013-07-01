function plot_data_from_log(in_file,calibration_file,min_sensitivity)

if (length(in_file)==0)
    return;
end

close all

calibration_file = 'microbar_calibration.mat';
min_sensitivity = 0.4;

load(calibration_file);

SMOOTHING_SPAN = 500;
BASELINE_TIME_1 = 20; %in minutes...the stable point at which we will measure the baseline signal and subtract
BASELINE_TIME_2 = 30;

PLOT_COLOR = zeros(1,96); %setting a well equal to 0 means that it won't plot

%this loop just sets all the column pairs to different colors
%you can also just set each one individually, i.e. PLOT_COLOR(56) = 'k'
%would set well 56 to black.
for i = 1:96
    switch (mod(i,12))
        case {1,2}
            PLOT_COLOR(i) = 'k';
        case {3,4}
            PLOT_COLOR(i) = 'g';
        case {5,6}
            PLOT_COLOR(i) = 'b';
        case {7,8}
            PLOT_COLOR(i) = 'm';
        case {9,10}
            PLOT_COLOR(i) = 'c';
        case {11,0}
            PLOT_COLOR(i) = 'r';
    end
end

%creates a MAT file for faster data access in the future
if (isempty(dir(strrep(in_file,'.log','.mat'))))
    fid = fopen(in_file);
    i = 0;
    assay_started = 0;
    well = zeros(96,1);
    while (1)
        line = fgetl(fid);
        if line==-1
            break;
        end
        [name, val] = strtok(line);

        %don't start populating the output array until the first timestamp is
        %found
        if strcmp(name,'assay_time')
            i = i+1;
            time(i) = str2num(val)/(60*1000);
        elseif (i == 0)
            continue;
        end

        switch (name)       
            case 'assay_state'
                state(i) = str2num(val);
            case 'assay_step'
                step(i) = str2num(val);            
            case 'pt'
                [idx, intens] = strtok(val);
                well(str2num(idx),i) = str2num(intens)/512;
            case 'batt_voltage'
                batt(i) = str2num(val);             
            case 'heater_temp'
                temp(i) = str2num(val);            
            case 'heater_on'
                heater_on(i) = str2num(val);            
            case 'temp_ok'
                temp_ok(i) = str2num(val);              
        end

        %disp(line);
    end
    
    fl_mean = mean(well');
    fl_stddev = std(well');
    
    save(strrep(in_file,'.log','.mat'),'well','time','state','step','temp','batt','heater_on','temp_ok','fl_mean','fl_stddev');    
else
    load(strrep(in_file,'.log','.mat')); %loads the MAT file, if it exists
end


figure(1)
subplot(2,1,1)
plot(time,state);
ylabel('State');
xlabel('Time (min)');
subplot(2,1,2)
plot(time,step);
ylabel('Step');
xlabel('Time (min)');

figure(2)
subplot(3,1,1)
plot(time,temp);
ylabel('Temp (C)');
xlabel('Time (min)');
subplot(3,1,2)
plot(time,batt);
ylabel('Batt (V)');
xlabel('Time (min)');
subplot(3,1,3)
plot(time,heater_on);
ylabel('Heater On');
xlabel('Time (min)');

run_indicies = find(state==4); % only plot data from the actual fluorescence run

wells_to_plot = find(PLOT_COLOR~=0);

figure(3);
f3 = gca;
hold all 
figure(4);
f4 = gca;
hold all    
figure(5);
f5 = gca;
hold all  
figure(6);
f6 = gca;
hold all
figure(7);
f7a = subplot(3,2,1);
hold all
f7b = subplot(3,2,2);
hold all
f7c = subplot(3,2,3);
hold all
f7d = subplot(3,2,4);
hold all
f7e = subplot(3,2,5);
hold all
f7f = subplot(3,2,6);
hold all

inlet_averages = zeros(6,length(run_indicies));
inlet_counts = zeros(6,1);

for i = 1:length(wells_to_plot)

    style = char(PLOT_COLOR(wells_to_plot(i)));
    name = num2str(wells_to_plot(i));
    plot(f3,time(run_indicies),well(wells_to_plot(i),run_indicies),'Color',style,'DisplayName',name);

    smoothed = smooth(well(wells_to_plot(i),run_indicies),SMOOTHING_SPAN);
    plot(f4,time(run_indicies),smoothed,'Color',style,'DisplayName',name);

    baseline_idx_1 = find(time(run_indicies)>BASELINE_TIME_1,1);
    baseline_idx_2 = find(time(run_indicies)>BASELINE_TIME_2,1);
    
    baseline_1 = smoothed(baseline_idx_1);
    baseline_2 = smoothed(baseline_idx_2);
    baseline_slope = (baseline_2 - baseline_1) / (baseline_idx_2 - baseline_idx_1);
    
    subtracted = smoothed - baseline_1 - ([1:length(smoothed)]-baseline_idx_1)'*baseline_slope;
    plot(f5,time(run_indicies),subtracted,'Color',style,'DisplayName',name);
    
    if (mean_sensitivity(wells_to_plot(i))>min_sensitivity)
        %fig 6
        normalized = subtracted*mean_sensitivity(wells_to_plot(i));
        plot(f6,time(run_indicies),normalized,'Color',style,'DisplayName',name);
    
    
        %fig 7
        switch (mod(wells_to_plot(i),12))
            case {1,2}
                plot(f7a,time(run_indicies),normalized,'DisplayName',name);
                inlet_averages(1,:) = inlet_averages(1,:) + normalized';
                inlet_counts(1) = inlet_counts(1) + 1;
            case {3,4}
                plot(f7b,time(run_indicies),normalized,'DisplayName',name);
                inlet_averages(2,:) = inlet_averages(2,:) + normalized';   
                inlet_counts(2) = inlet_counts(2) + 1;                
            case {5,6}
                plot(f7c,time(run_indicies),normalized,'DisplayName',name);
                inlet_averages(3,:) = inlet_averages(3,:) + normalized';   
                inlet_counts(3) = inlet_counts(3) + 1;                
            case {7,8}
                plot(f7d,time(run_indicies),normalized,'DisplayName',name);
                inlet_averages(4,:) = inlet_averages(4,:) + normalized';
                inlet_counts(4) = inlet_counts(4) + 1;                
            case {9,10}
                plot(f7e,time(run_indicies),normalized,'DisplayName',name);
                inlet_averages(5,:) = inlet_averages(5,:) + normalized';  
                inlet_counts(5) = inlet_counts(5) + 1;                
            case {11,0}
                plot(f7f,time(run_indicies),normalized,'DisplayName',name);
                inlet_averages(6,:) = inlet_averages(6,:) + normalized';   
                inlet_counts(6) = inlet_counts(6) + 1;                
        end
        
    end
end


figure(3)
title('Raw Signals');
ylabel('Fluorescence (AU)');
xlabel('Time (min)');
axis([min(time(run_indicies)),max(time(run_indicies)),0,1])


figure(4);
title(sprintf('Smoothed, span =%d',SMOOTHING_SPAN));
ylabel('Fluorescence (AU)');
xlabel('Time (min)');
axis([min(time(run_indicies)),max(time(run_indicies)),0,1])

figure(5);
title(sprintf('After Baseline Subtraction'));
ylabel('Fluorescence (AU)');
xlabel('Time (min)');
axis([min(time(run_indicies)),max(time(run_indicies)),-.1,1])

figure(6);
title(sprintf('After Normalization'));
ylabel('Fluorescence (AU)');
xlabel('Time (min)');
axis([min(time(run_indicies)),max(time(run_indicies)),-.1,1])

figure(7);
title(f7a,sprintf('Inlet 1'));
title(f7b,sprintf('Inlet 2'));
title(f7c,sprintf('Inlet 3'));
title(f7d,sprintf('Inlet 4'));
title(f7e,sprintf('Inlet 5'));
title(f7f,sprintf('Inlet 6'));
axis(f7a,[min(time(run_indicies)),max(time(run_indicies)),-.1,1])
axis(f7b,[min(time(run_indicies)),max(time(run_indicies)),-.1,1])
axis(f7c,[min(time(run_indicies)),max(time(run_indicies)),-.1,1])
axis(f7d,[min(time(run_indicies)),max(time(run_indicies)),-.1,1])
axis(f7e,[min(time(run_indicies)),max(time(run_indicies)),-.1,1])
axis(f7f,[min(time(run_indicies)),max(time(run_indicies)),-.1,1])

%averages per inlet
figure(8)
for i=1:6
    subplot(3,2,i)
    inlet_averages(i,:) = inlet_averages(i,:) / inlet_counts(i);
    plot(time(run_indicies),inlet_averages(i,:),'DisplayName',sprintf('Inlet %d',i));
    
end

figure(9)
for i=1:96
    subplot(8,12,i)
    plot(time(run_indicies),well(i,run_indicies),'Color','k','DisplayName',name);
    axis([min(time(run_indicies)),max(time(run_indicies)),0.2,0.6])
end

disp(mean(well(15,:)));
