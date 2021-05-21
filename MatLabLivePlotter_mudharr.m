%MATALB Live Plotter for 2DP4 Project
%By Raj Mudhar, mudharr, 400088877

clear;
%Delete empty serial ports to stop connection issues
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

%Create new serial object and open serial port
s = serial('COM2');
set(s,'BaudRate',38400); % set baud rate to 38400
fopen(s);

%time variables to sample for interv length
init_time = 1;
interv = 1000; %1000 samples at 2Hz
x = 0; %graph data

%scan port and make live graph
while (init_time < interv)
    out = fscanf(s,'%d');
    x = [x,out];
    plot(x);
    title("Angle Reading, Raj Mudhdar, 400088877");
    grid ON
    init_time = init_time+1;
    drawnow
end