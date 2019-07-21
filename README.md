# FRS-Transceiver
Repository contains GRC and Matlab Directories.

## Cloning
git clone https://github.com/Abdob/FRS-Transceiver.git


## Updating
cd FRS-Transceiver/

git pull

## Generating alternating pulses to examine bladerf dual rx
We examine the received signal when transmitting pulses. The pulse to be transmitted from another radio will be a square-root raised cosine pulse, which is commonly used in digital communications.

![GitHub Logo](/Diagrams/SRRC.jpg)

The pulse is generated and a sequence of alternating pulses from positive to negative is generated in Matlab and subsequently stored in a data file to transmit from GRC.

![GitHub Logo](/Diagrams/Sequence.jpg)

## Examining single and dual rx

![GitHub Logo](/Diagrams/single_RX1.jpg)

![GitHub Logo](/Diagrams/dual_RX1.jpg)

## BladeRF RX Source Internal

![GitHub Logo](/Diagrams/BladeRF_RX.jpg)

## Building libbladerf from source

If bladeRF was installed with sudo apt-get, uninstall it with the following command:

sudo apt-get remove bladerf

sudo apt-get remove libbladerf-dev

Ensure the bladeRF library isn't in the system:

sudo find / -name "libbladeRF.so"

Go to the step where the dependencies are installed then the step to build libbladerf from source:

https://github.com/Nuand/bladeRF/wiki/Getting-Started%3A-Linux

## Building osmosdr from source

When building gr-osmosdr rx/tx source block, cmake looks for the bladerf library, libbladeRF.so, to link and finish compiling. Thus, BladeRF library is build prior to osmosdr. The source code which interfaces osmosdr with the bladerf library is in the gr-osmosdr/lib/bladerf directory. The directory contains an api with functions to access and control the bladerf. This source code can be changed and compiled if needed. 

https://github.com/osmocom/gr-osmosdr

When running the GRC application with the osmosdr source block, debugging information from gr-osmosdr/lib/bladerf/bladerf_source_c.cc and bladerf_common.cc as shown below. bladerf_source_c.cc has been slight modified and includes a print statement for configuring each channel.

[bladeRF common] bladerf_source_c: Configuring Channel: 0

[bladeRF common] bladerf_source_c: Configuring Channel: 1

[bladeRF common] init: DEBUG: entering initialization

[bladeRF source] init: Opening Nuand bladeRF with device identifier string '*:instance=0'

[bladeRF source] Device: Nuand bladeRF 2.0 Serial # 3519...74df FW v2.3.2 FPGA v0.10.2

[bladeRF source] init: Buffers: 512, samples per buffer: 4096, active transfers: 32

[bladeRF source] bladerf_source_c: DEBUG: initialization complete

[bladeRF source] start: DEBUG: starting source

[bladeRF source] stop: DEBUG: stopping source

## Bypassing osmo-sdr for trouble-shooting

To Rule out the issue is in osmo-sdr, we bypass this block and analyze what would be received from the libbladeRF library.

Enter the bladeRF command line interface:

bladeRF-cli -i

View your radio's configuration and set frequency and sample rate to match the trasnmitter:

print

set frequency 2100000000

set samplerate 2000000

Capture samples from your transmitting radio:

rx config file=samples.csv format=csv n=4M

rx start

On Matlab Right Click on the samples.csv file and use the Import Data tool. You will get table data named 'samples'. Verify this by typing this on Matlab:

>> class(samples)

Extract the first column and the second column of the table data which are your inphase component and the quadrature component of your received data respectively:

>> col1 = samples{:,1};

>> col2 = samples{:,2};

Now col1 and col2 are of type 'double', plot them (500 samples) using:

>> subplot(211)

>> plot(col1(4500:5000))

>> title('rx config file=samples.csv format=csv n=4M')

>> subplot(212)

>> plot(col2(4500:5000))

>> title('Quadrature component')

![GitHub Logo](/Diagrams/bladeRF_Single_RX.jpg)

Now with the bladeRF set to the frequency and sample rate in the bladeRF-cli program as described above, configure the bladeRF to mimo and collect the samples again (in the same procedure as above there is an external radio transmitting the alternating pulses):

rx config file=mimo.csv format=csv n=32768 channel=1,2

rx start

The mimo.csv file containing the samples are produced and ported to Matlab, this time you will see the table data containing four columns as opposed to two as before. This is due to having two receive ports each containing inphase and quadrature component totaling to four total components. We are ploting channel 1 output in Matlab again:

>> col1 = mimo{:,1};

>> col2 = mimo{:,2};

>> col3 = mimo{:,3};

>> col4 = mimo{:,4};

>> subplot(211)

>> plot(col1(7000:7500))

>> title('rx config file=mimo.csv format=csv n=32768 channel=1,2')

>> subplot(212)

>> plot(col2(7000:7500))

>> title('Channel 1: Quadrature component')

![GitHub Logo](/Diagrams/bladeRF_Multiple_RX.jpg)







https://github.com/Nuand/bladeRF/tree/master/host/utilities/bladeRF-cli



## Other Links

https://github.com/Nuand/bladeRF/wiki/Getting-Started:-Verifying-Basic-Device-Operation

https://github.com/Nuand/bladeRF/wiki/Gnuradio-OsmoSDR-device-string-options

https://github.com/Nuand/bladeRF/tree/dev-hdl-rx-fifo-fixes

