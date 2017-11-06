clear all; close all; clc;

%% Read data from CSV in the same folder
thresh = csvread('scores.subsumption-v2.threshold.csv',1,0);
eps = csvread('scores.subsumption-v2.epsilon_greedy.csv',1,0);
inv = csvread('scores.subsumption-v2.inverse_proportional.csv',1,0);

%% Generate plot
n = 100;
xmin = 0;
xmax = 1000;
f = @(X,n) filter(ones(1,n)./n,eye(n,1),X);

plot(thresh(:,1),f(thresh(:,2),n), ...
    eps(:,1),f(eps(:,2),n), ...
    inv(:,1),f(inv(:,2),n));
xlabel('Training Games');
ylabel('Score');
xlim([xmin xmax]);
legend('threshold','epsilon\_greedy','inverse\_proportional','Location','northwest');
grid on;

saveas(gcf,'exploration.eps','epsc');
