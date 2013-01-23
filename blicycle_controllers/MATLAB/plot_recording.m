%% Load in the data
clear; clc;
blicycle = load('../data/recording-1.csv');

t = blicycle(:, 1);
thetaSteer = blicycle(:, 2);
x = blicycle(:, 3);
y = blicycle(:, 4);
theta = blicycle(:, 5);
error = blicycle(:, 6);
beta = blicycle(:, 7);
correction = blicycle(:, 8);

%% Create a trajectory plot


plot(correction, y, 'r-');
hold on
plot([0 0], [min(y) max(y)], 'g--', 'LineWidth', 2);
plot(x, y, 'LineWidth', 2);
hold off

title('Simulated Bicycle Trajectory');
axis equal;
axis([-8 8 0 40]);
xlabel('X position (m)');
ylabel('Y position (m)');
set(1, 'Color', 'w');