clear all; close all; clc;

%% Read data from CSV in the same folder
s6236 = csvread('performance-6236.csv',1,0);
s318534 = csvread('performance-318534.csv',1,0);
s475133 = csvread('performance-475133.csv',1,0);
s482403 = csvread('performance-482403.csv',1,0);
s984110 = csvread('performance-984110.csv',1,0);

%% Generate table
format long;
disp('seed 6236');
disp(int32(mean(s6236(1:10,2))));
disp('seed 318534');
disp(int32(mean(s318534(1:10,2))));
disp('seed 475133');
disp(int32(mean(s475133(1:10,2))));
disp('seed 482403');
disp(int32(mean(s482403(1:10,2))));
disp('seed 984110');
disp(int32(mean(s984110(1:10,2))));
