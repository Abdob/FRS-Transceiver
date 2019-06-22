function [] = DataToGRC(filename, data, type)
    % Writing a data file so it can be read from Gnu Radio
    % function [] = DataToGRC(filename, data, type)
    filename = strcat('../GRC/', filename, '.dat');
    f = fopen (filename, 'wb'); 
    switch(type)
        case 'uint8'
            data = uint8(data);
            fwrite (f, data , 'unsigned char');
        case 'single'
            data = single(data);
            fwrite (f, data , 'float');
    end  
    fclose (f);
    
end
    