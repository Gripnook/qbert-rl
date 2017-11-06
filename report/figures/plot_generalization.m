clear all; close all; clc;

%% Read data from CSV in the same folder
mono = csvread('scores.monolithic.threshold.csv',1,0);
sub1 = csvread('scores.subsumption-v1.threshold.csv',1,0);
sub2 = csvread('scores.subsumption-v2.threshold.csv',1,0);
sub3 = csvread('scores.subsumption-v3.threshold.csv',1,0);

%% Generate plot
n = 100;
xmin = 0;
xmax = 3000;
f = @(X,n) filter(ones(1,n)./n,eye(n,1),X);

plot(mono(:,1),f(mono(:,2),n), ...
    sub1(:,1),f(sub1(:,2),n), ...
    sub2(:,1),f(sub2(:,2),n), ...
    sub3(:,1),f(sub3(:,2),n));
xlabel('Training Games');
ylabel('Score');
xlim([xmin xmax]);
legend('monolithic','subsumption-v1','subsumption-v2','subsumption-v3','Location','northwest');
grid on;

saveas(gcf,'generalization.eps','epsc');
