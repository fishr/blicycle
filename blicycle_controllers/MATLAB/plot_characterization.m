%% Load in the data
clear; clc;
blicycle = load('/home/steve/Dropbox/PPAT Docs/BikeSimulator/BlicycleSimulator/recording3.csv');

t = blicycle(:, 1);
thetaSteer = blicycle(:, 2);
desiredAngle = blicycle(:, 3);
correction = blicycle(:, 4);

%% Create a trajectory plot


plot(t, thetaSteer, 'r-');

title('Human Operator Step Response');
xlabel('Time (s)');
axis([0 20 -0.3 0.5]);
ylabel('Angular position (rad)');
set(1, 'Color', 'w');