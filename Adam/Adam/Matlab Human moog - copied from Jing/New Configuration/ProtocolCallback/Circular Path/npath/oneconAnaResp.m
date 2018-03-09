function oneconAnaResp(appHandle)

global  debug %connected

if debug
    disp('Entering oneconAnaResp')
end

Resp = getappdata(appHandle,'ResponseInfo');
data = getappdata(appHandle,'protinfo');

crossvals = getappdata(appHandle, 'CrossVals');
crossvalsGL = getappdata(appHandle, 'CrossValsGL');
trial = getappdata(appHandle, 'trialInfo');
cntr = trial.list(trial.cntr);


stairInfo = getappdata(appHandle,'stairInfo');
%response = Resp(stairInfo.currStair).response(stairInfo.trialCount(stairInfo.currStair))

% Resp(stairInfo.currStair).samediff(stairInfo.trialCount(stairInfo.currStair)) = getappdata(appHandle,'samediff');
% Resp(stairInfo.currStair).intOrder(:,stairInfo.trialCount(stairInfo.currStair)) = getappdata(appHandle,'Order');
response = Resp(data.repNum).response(trial.cntr);
Resp(data.repNum).samediff(trial.cntr) = getappdata(appHandle,'samediff');
%Resp(data.repNum).intOrder(:,trial.cntr) = getappdata(appHandle,'Order');

i = strmatch('RADIUS',{char(data.configinfo.name)},'exact'); %RADIUS means standard radius
if data.configinfo(i).status == 2
    i1 = strmatch('Radius of Curve',{char(data.condvect.name)},'exact');
    r(1,1) = crossvals(cntr,i1);
    r(2,1) = crossvalsGL(cntr,i1);
else
    r(1,1) = data.configinfo(i).parameters.moog;
    r(2,1) = data.configinfo(i).parameters.openGL;
end


i = strmatch('MOTION_TYPE',{char(data.configinfo.name)},'exact');

% SINGLE INTERVAL // We should never enter this part
if data.configinfo(i).parameters == 1
    %     dir(2) = stairInfo.stairs(stairInfo.currStair);
    %     Resp(data.repNum).dir(:,trial.cntr) = [dir(2)];
    amp(2) = r(1,1);      %stairInfo.steps(stairInfo.currStep(stairInfo.currStair));
    Resp(data.repNum).amp(:,trial.cntr) = [amp(2)];

    if response == 1 % Left button press = first
        if debug
            disp('You answered rotation')
        end
        if Resp(data.repNum).samediff(trial.cntr)==1 % it is rotation
            if debug
                disp('Correct')
            end
            Resp(data.repNum).corr(trial.cntr) = 1;
            Resp(data.repNum).incorr(trial.cntr) = 0;
            Resp(data.repNum).null(trial.cntr) = 0;
            Resp(data.repNum).dontKnow(trial.cntr) = 0;
        elseif Resp(data.repNum).samediff(trial.cntr)==2 % it is curve path
            if debug
                disp('Not correct')
            end
            Resp(data.repNum).corr(trial.cntr) = 0;
            Resp(data.repNum).incorr(trial.cntr) = 1;
            Resp(data.repNum).null(trial.cntr) = 0;
            Resp(data.repNum).dontKnow(trial.cntr) = 0;
        else
            if debug
                disp('No Answer')
            end
            Resp(data.repNum).corr(trial.cntr) = 0;
            Resp(data.repNum).incorr(trial.cntr) = 0;
            Resp(data.repNum).null(trial.cntr) = 1;
            Resp(data.repNum).dontKnow(trial.cntr) = 0;
        end

    elseif response == 2 %anwser curve path
        if debug
            disp('You answered curve path')
        end
        %%%%%%%%%%%%%%%
        if Resp(data.repNum).samediff(trial.cntr)==2 % it is curve path
            if debug
                disp('Correct')
            end
            Resp(data.repNum).corr(trial.cntr) = 1;
            Resp(data.repNum).incorr(trial.cntr) = 0;
            Resp(data.repNum).null(trial.cntr) = 0;
            Resp(data.repNum).dontKnow(trial.cntr) = 0;
        elseif Resp(data.repNum).samediff(trial.cntr)==1  % it is rotation
            if debug
                disp('Not correct')
            end
            Resp(data.repNum).corr(trial.cntr) = 0;
            Resp(data.repNum).incorr(trial.cntr) = 1;
            Resp(data.repNum).null(trial.cntr) = 0;
            Resp(data.repNum).dontKnow(trial.cntr) = 0;
        else
            if debug
                disp('No Answer')
            end
            Resp(data.repNum).corr(trial.cntr) = 0;
            Resp(data.repNum).incorr(trial.cntr) = 0;
            Resp(data.repNum).null(trial.cntr) = 1;
            Resp(data.repNum).dontKnow(trial.cntr) = 0;
        end

    else % Unrecognized answer
        if debug
            disp('Unrecognized answer')
        end
        Resp(data.repNum).corr(trial.cntr) = 0;
        Resp(data.repNum).incorr(trial.cntr) = 0;
        Resp(data.repNum).null(trial.cntr) = 1;
        Resp(data.repNum).dontKnow(trial.cntr) = 0;
    end
end

%%%%%%%%%%%%%%%%%%
Resp(data.repNum).totalCorr = sum(Resp(data.repNum).corr);
Resp(data.repNum).totalIncorr = sum(Resp(data.repNum).incorr);
Resp(data.repNum).totalNull = sum(Resp(data.repNum).null);
Resp(data.repNum).totalDontKnow = sum(Resp(data.repNum).dontKnow);

Resp(data.repNum)
setappdata(appHandle,'ResponseInfo',Resp);
%+++++++++ Commented out for now, put in analyzeResp+++++++++++++++++++++++

if debug
    if Resp(data.repNum).corr(trial.cntr) == 1
        %%%%%%%%%%%
        soundsc(data.correctWav,42000);
    else
        soundsc(data.wrongWav,42000);
    end
    disp('Exiting oneconAnaResp')
end


