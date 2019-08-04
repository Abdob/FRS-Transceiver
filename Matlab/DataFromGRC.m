function [v] = DataFromGRC(filename,type ,len)
    % Reading from an GNU Radio generated data file
    % function [v] = DataFromGRC(filename,type ,len)
    % file name: example.dat will be 'example'
    % type is 'uint8', 'float' or 'complex'

    if nargin<3
      len = inf;
    end
    
    filename = strcat('../GRC/',filename,'.dat');
    f = fopen(filename, 'rb');
    switch(type)
        case 'uint8'
            v = fread(f,len,'uint8');
            v = v';
        case 'complex'
            r = fread(f,len,'float');
            v = single(r(1:2:end)) + j*single(r(2:2:end));
    end
    fclose(f);
end
