%% Calibration helper script
clear; clc;
A = importdata('calib_data.txt');

x = A(:, 1);
theta = A(:, 2);
rho = A(:, 3);
phi = A(:, 4);

rhostep = 0.01;
phistep = 0.01;

rhoi = (min(rho)-rhostep):rhostep:(max(rho)+rhostep);
phii = (min(phi)-phistep):phistep:(max(phi)+phistep);

[RHOI PHII] = meshgrid(rhoi, phii);

XI = griddata(rho, phi, x, RHOI, PHII);
THETAI = griddata(rho, phi, theta, RHOI, PHII);


%% Generate plots
subplot(1, 2, 1);
surf(RHOI, PHII, XI);
hold on;
plot3(rho, phi, x, 'o');
hold off;
title('X grid data');
xlabel('\rho (m)');
ylabel('\phi (rad)');
zlabel('X (m)');
set(1, 'Color', 'w');

subplot(1, 2, 2);
surf(RHOI, PHII, THETAI);
hold on;
plot3(rho, phi, theta, 'o');
hold off;
title('\theta grid data');
xlabel('\rho (m)');
ylabel('\phi (rad)');
zlabel('\theta (deg)');

%% Write file data

% Set all NAN's to 0's
XI(find(isnan(XI))) = 0;
THETAI(find(isnan(THETAI))) = 0;

csvwrite('rhoi.txt', rhoi);
csvwrite('phii.txt', phii);
csvwrite('xi.txt', XI);
csvwrite('thetai.txt', THETAI);
