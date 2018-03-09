function FitPsychometricOnline(appHandle)

if isempty(findobj('Name','Fit Psychometric'))
    set(0,'Units','centimeters')
    pos = get(0,'ScreenSize');

    debugfig = figure('Units','centimeters',...
        'Position',[pos(3)-6.1 2 5 2],...
        'Color',[0.831 0.816 0.784],...
        'Name','Fit Psychometric',...
        'NumberTitle','off',...
        'MenuBar','none');

    uicontrol(debugfig,'Units','centimeters',...
        'Style','pushbutton',...
        'Position',[1 0.5 3 1],...
        'String','Fit Psychometric',...
        'Callback',{@runFit_callback});
end

% plotData = getappdata(appHandle,'psychPlot');
%
% save('C:\Program Files\MATLAB\R2006a\work\New Configuration\ProtocolCallback\TargetDiscrimination\data_for_psycho_fit.mat')

function runFit_callback(hObject, eventdata)

load 'C:\Program Files\MATLAB\R2006a\work\New Configuration\ProtocolCallback\TargetDiscrimination\data_for_psycho_fit.mat'


directions = unique(savedInfo.Resp.dir);
directions_zero_index = find(directions == 0);

if ~isempty(directions_zero_index) %i.e. if there is a zero direction or orientation ...
    directions(directions_zero_index) = []; %remove the zero
end

% % % % % % % % % % % % % % % % %  struct array initialization   %%%%%%%
data_organized.name = 'initialization'; %% this is just an initialization of the struct array
data_organized(2).name = 'initialization';
% % % % % % % % % % % % % %   struct array initialization end  %%%%%%



for i = 1:length(directions)

    data_organized(i).name = ['n' num2str(directions(i))];
    data_organized(i).direction_value = directions(i);
    data_organized(i).amt_correct = 0;
    data_organized(i).amt_incorrect = 0;
    data_organized(i).amt_null = 0;
    data_organized(i).amt_dontKnow = 0;
    data_organized(i).direction_total = 0;
    % %        data_organized(i).percent_left = 0;
    % %        data_organized(i).percent_right = 0;
    data_organized(i).percent_correct = 0;
    data_organized(i).percent_incorrect = 0;
    data_organized(i).amt_rightc = 0;
    data_organized(i).amt_leftc = 0;
end


for m = 1: length(savedInfo) %number of staircases
    for i = 1: length(savedInfo(m).Resp) % # of repetitions

        for j = 1:length(savedInfo(m).Resp(1,1).response)  %%# of trials

            for k = 1: length(data_organized)

                respon = savedInfo(m).Resp(1, i).response(j);   % subject response

                direction = savedInfo(m).Resp(1, i).dir(j) ;


                ddir(j) = direction;

                %direction of current trial
                a_correct_response = savedInfo(m).Resp(1, i).corr(j);       %subject's correct responses
                an_incorrect_response = savedInfo(m).Resp(1, i).incorr(j) ;    %subject's incorrect responses
                a_null_response = savedInfo(m).Resp(1, i).null(j) ;      %subject's null responses
                an_i_dont_know_response = savedInfo(m).Resp(1, i).dontKnow(j) ;  %subject's "I don't know" responses
                %         reversal =
                %         savedInfo.Rep.Trial(i).Param(14).value
                %         %eiter regular or reverse trial
                %                     reaction_time_counter = reaction_time_counter + 1;

                if (direction == data_organized(1, k).direction_value)


                    if respon == 2
                        data_organized(1, k).amt_rightc = data_organized(1, k).amt_rightc + 1;
                    end

                    if respon == 1
                        data_organized(1, k).amt_leftc = data_organized(1, k).amt_leftc + 1;
                    end

                    data_organized(1, k).amt_correct = data_organized(1, k).amt_correct + a_correct_response;
                    data_organized(1, k).amt_incorrect = data_organized(1, k).amt_incorrect + an_incorrect_response;
                    data_organized(1, k).amt_null = data_organized(1, k).amt_null + a_null_response;
                    data_organized(1, k).amt_dontKnow = data_organized(1, k).amt_dontKnow + an_i_dont_know_response;


                end

            end

        end

    end

end

for i = 1:length(data_organized)
    data_organized(1, i).direction_total = data_organized(1, i).amt_correct + data_organized(1, i).amt_incorrect;
end



for i = 1:length(data_organized)

    data_organized(1, i).direction_total = data_organized(1, i).amt_correct + data_organized(1, i).amt_incorrect;
    data_organized(1, i).percent_correct = data_organized(1, i).amt_correct/data_organized(1, i).direction_total;
    data_organized(1, i).percent_rightc = data_organized(1, i).amt_rightc/data_organized(1, i).direction_total;%this line
    data_organized(1, i).percent_leftc = data_organized(1, i).amt_leftc/data_organized(1, i).direction_total;

    if data_organized(1, i).direction_total == 0
        data_organized(i).direction_value %%this shows the direction that has no data
    end
    data_organized(1, i).percent_incorrect = data_organized(1, i).amt_incorrect/data_organized(1, i).direction_total;
    if data_organized(1, i).direction_total == 0
        data_organized(i).direction_value  %%this shows the direction that has no data
    end


end

for i = 1:length(data_organized)
    percentage_correct(i) = data_organized(1, i).percent_correct;
    percentage_rightc(i) = data_organized(1, i).percent_rightc;%this line
    percentage_leftc(i) = data_organized(1, i).percent_leftc;
end



for i = 1:length(data_organized)
    direction_total(i) = data_organized(i).direction_total;
end


pfit_percentage_left_format = cat(2, directions', percentage_rightc', direction_total' );




% % removing self inputted NaNs to get rid of empty trials

pfit_percentage_left_format_NaN = isnan(pfit_percentage_left_format);
pfit_percentage_left_format_NaN = sum(pfit_percentage_left_format_NaN, 2);
NaN_rows = find(pfit_percentage_left_format_NaN);
k = length(NaN_rows) + 1; %%initialization to count backwards in the following for loop
for j = 1:length(NaN_rows)
    k = k - 1;
    %         pfit_percentage_left_format(NaN_rows(k), :) = [];
    pfit_percentage_left_format(NaN_rows(k), :) = []; %getting rid of rows with NaNs i.e. the empty trials
    %         datasets_all(i).direction_data_stderror(NaN_rows(k)) = [];  % getting rid of the concurrent calculated std error otherwise the plots will get screwed up
end

%     datasets_all(i).direction_data_mean;  %debugging
%     if exist(NaN_rows)
%         datasets_all(i).direction_data_mean = pfit_percentage_left_format
%     end






pfit_output = pfit(pfit_percentage_left_format,'plot_opt','no plot','shape','cumulative gaussian','n_intervals',1,'FIX_LAMBDA',0.001,'sens',0,'compute_stats','false','verbose','false');

params_mu_sigma = [pfit_output.params.est(1), pfit_output.params.est(2)];
sig = pfit_output.params.est(2);
pse = norminv(0.5,pfit_output.params.est(1), pfit_output.params.est(2));


online_plot_handle = figure(10); % -- i.e. the online psychometric plot
online_plot_handle = subplot(311);
hold on


xi = .1 : .1 : 32;
n_xi= fliplr(-xi);
xi = cat(2, n_xi, xi);
% %   xi vector used to plot the fitted sigmoidal curve parameters on top
% of the data
plot(pfit_percentage_left_format(:,1)', pfit_percentage_left_format(:,2)', 'bo', xi, cum_gaussfit(params_mu_sigma , xi),'b-' );
xlabel('Tilt Direction (degrees)');

ylabel('Percentage Rightward choices (%)');
% title('test fit');

xlimits = get(online_plot_handle, 'xlim');
ylimits = get(online_plot_handle, 'ylim');
xlimits_mean = mean(xlimits);
ylimits_mean = mean(ylimits);

text(xlimits_mean, (ylimits_mean - 0.1), ['Sigma:'  '  ' num2str(sig)]);
text(xlimits_mean, ylimits_mean, ['Pse:' '  ' num2str(pse)]);

hold on;
for i=1:50
    xl(i)=0;
    yl(i)=0.02*i;
end
plot(xl,yl,'r-');
hold off;

pse_round = round(pse);
% this section is for Static_Tilt_Ref_MuliRule
load 'Z:\Users\Dylan\TestSetup\HumanMoog\New Configuration\Protocols\Static_Tilt_Ref_MultiRule.mat'
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% for i = 1:length(config.variables)
if (tunnel_elevation_status == 4 && tunnel_roll_status == 1)
    %     if strcmp(config.variables(i).nice_name, 'Tunnel Roll')
    %     if strcmp(config.variables(i).nice_name, 'Tunnel Elevation')
    i = strmatch('Tunnel Elevation', {char(config.variables.nice_name)},'exact');
elseif (tunnel_elevation_status == 1 && tunnel_roll_status == 4)
    i = strmatch('Tunnel Roll' ,{char(config.variables.nice_name)},'exact');
else
    disp('you cannot fit Psychometric to two varying parameters. Either roll or pitch should remain constant')
end
config.variables(i).parameters = pse_round;
disp(['PSE set to ' num2str(pse_round)])
config.variables(i).low_bound = -30 + pse_round;
config.variables(i).high_bound = 30 + pse_round;
disp('Log range has been set to +/-30 wrt PSE ')
%     end
% end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
save('Z:\Users\Dylan\TestSetup\HumanMoog\New Configuration\Protocols\Static_Tilt_Ref_MultiRule.mat', 'config')

% this section is for Bar_Static_Tilt_Ref_MuliRule
load 'Z:\Users\Dylan\TestSetup\HumanMoog\New Configuration\Protocols\Bar_Static_Tilt_Ref_MultiRule.mat'
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% for i = 1:length(config.variables)
if (tunnel_elevation_status == 4 && tunnel_roll_status == 1)
    %     if strcmp(config.variables(i).nice_name, 'Tunnel Roll')
    %     if strcmp(config.variables(i).nice_name, 'Tunnel Elevation')
    i = strmatch('Tunnel Elevation', {char(config.variables.nice_name)},'exact');
elseif (tunnel_elevation_status == 1 && tunnel_roll_status == 4)
    i = strmatch('Tunnel Roll' ,{char(config.variables.nice_name)},'exact');
else
    disp('you cannot fit Psychometric to two varying parameters. Either roll or pitch should remain constant')
end
config.variables(i).parameters = pse_round;
disp(['PSE set to ' num2str(pse_round)])
config.variables(i).low_bound = -30 + pse_round;
config.variables(i).high_bound = 30 + pse_round;
disp('Log range has been set to +/-30 wrt PSE ')
%     end
% end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
save('Z:\Users\Dylan\TestSetup\HumanMoog\New Configuration\Protocols\Bar_Static_Tilt_Ref_MultiRule.mat', 'config')



disp('writing data to "1-down, 2-up"/"2-down, 1-up" Static_Tilt_Ref_Multirule... ')





%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%     % % % % % % % % % % % % %  MAIN CODE END % % % % % % % % % % % % % % % % %
%     % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % %
%
clear NaN_rows savedInfo a_correct_response a_null_response ...
    abs_direction an_i_dont_know_response an_incorrect_response data_organized ddir ...
    direction direction_max direction_min direction_total directions_int ...
    directions_int_flip directions_int_left_right directions_left directions_right ...
    directions_right_int i j k n_xi num_directions params_mu_sigma percentage_correct ...
    percentage_left percentage_right percentage_rightc pfit_output pfit_percentage_left_format ...
    pfit_percentage_left_format_NaN print_on pse respon sig trans_direction two_interval xi xl yl




