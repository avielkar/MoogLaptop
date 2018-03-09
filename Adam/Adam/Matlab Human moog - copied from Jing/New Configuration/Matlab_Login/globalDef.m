% setup global variable when startup function call
global REPORTDIR LOGINFILE LOGFILE RECORDEMAIL ADMINEMAIL ADMINEMAIL2 SMTP_SERVER APPLICATION ...
    SUBJECT_NUM SUBJECT_DIR SUBJECT_FILE SUBJECT_PARA_DIR SUBJECT_PARA_FILE

%dir = 'C:\Program Files\MATLAB\R2006a\work\New Configuration\Matlab_Login\';
% dir = 'C:\User\Johnny\Matlab_Login\Matlab_Login_datebase\';
dir = 'C:\human moog\New Configuration\Matlab_Login\';  %for jing's computer

% where to keep the monthly report
REPORTDIR = dir;

% login file keep all user information
LOGINFILE =  [dir 'loginData.mat'];

% log file keep all login and logout time of user
LOGFILE = [dir 'logData.mat'];

% This person keeps monthly records and maintains Human Moog
% Protocol files issued by the Human Studies Committee
%RECORDEMAIL = 'mandy@pcg.wustl.edu'; % Mandy 
RECORDEMAIL = 'mturner@cns.bcm.edu'; % Mandy 


% Administrators' email
ADMINEMAIL = 'jianchen@cns.bcm.edu'; % Jian chen
ADMINEMAIL2 = 'jlin@cns.bcm.edu';%Jing Lin

% Email server
%SMTP_SERVER = 'pcg.wustl.edu';
SMTP_SERVER = 'cns.bcm.edu';

% Matlab appliction program
% APPLICATION = 'BasicInterface';

% Subject number
SUBJECT_NUM = -1;

% Subject file directory
% SUBJECT_DIR = 'C:\Program Files\MATLAB\R2006a\work\New Configuration\Subject_Context\';
 SUBJECT_DIR = dir;

% Subject file name
SUBJECT_FILE = [SUBJECT_DIR 'HMsubject.txt'];

% Subject parameter's directory
SUBJECT_PARA_DIR = 'C:\Program Files\MATLAB\R2006a\work\New Configuration\Subject_Context\';

% Subject parameter's file
SUBJECT_PARA_FILE = '';