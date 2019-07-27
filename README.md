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

## BladeRF RX Source Internal

![GitHub Logo](/Diagrams/BladeRF_RX.jpg)

## Trouble Shooting Inital Setup

![GitHub Logo](/Diagrams/Initial_troubleshooting_setup.jpg)

## Examining single and dual rx in Windows

Running the grc flowcharts applications single_RX and dual_RX we capture the samples stored in single_RX_1.dat, dual_RX_1.dat and dual_RX_2.dat, these are plotted below along with samples captured outside of GRC.

To bypass GRC enter the bladeRF command line interface:

bladeRF-cli -i

View your radio's configuration and set frequency and sample rate to match the trasnmitter:

print

set frequency 2100000000

set samplerate 2000000

set bandwidth 56000000

Capture samples from your transmitting radio in Single RX mode:

rx config file=single.csv format=csv n=4K

rx start

Now with the bladeRF set to the same frequency, sample rate and bandwidth in the bladeRF-cli program as described above, configure the bladeRF to mimo and collect the samples again (in the same procedure as above there is an external radio transmitting the alternating pulses):

rx config file=dual.csv format=csv n=8K channel=1,2

rx start

On Matlab Right Click on the produced files single.csv and dual.csv file and use the Import Data tool. You will get table data named 'single'and dual.

The mimo.csv file containing the samples are produced and ported to Matlab, this time you will see the table data containing four columns as opposed to two as before. This is due to having two receive ports each containing inphase and quadrature component totaling to four total components. We are ploting channel 1 output in Matlab again:

Run Matlab script 'ExamineAlternating.m'


![GitHub Logo](/Diagrams/GRC_Single_RX_Windows.jpg)

![GitHub Logo](/Diagrams/GRC_dual_RX1_Windows.jpg)

![GitHub Logo](/Diagrams/GRC_dual_RX2_Windows.jpg)

![GitHub Logo](/Diagrams/CLI_Single_RX_Windows.jpg)

![GitHub Logo](/Diagrams/CLI_dual_RX1_Windows.jpg)

![GitHub Logo](/Diagrams/CLI_dual_RX2_Windows.jpg)



## Building libbladerf from source --skip this (actually we are uninstalling libbladerf built from source and retesting the apt-get version)

If bladeRF was installed with sudo apt-get, uninstall it with the following command:

sudo apt-get remove bladerf

sudo apt-get remove libbladerf-dev

Ensure the bladeRF library isn't in the system:

 sudo apt-get install libbladerf-dev

Go to the step where the dependencies are installed then the step to build libbladerf from source:

https://github.com/Nuand/bladeRF/wiki/Getting-Started%3A-Linux

## If libbladerf was built from source uninstall it and reinstall using apt-get

cd host/build

sudo make uninstall 

Make sure it is uninstalled:

sudo apt-get install libbladerf-dev

delete any libbladeRF.so library files found.

Install from new:

sudo add-apt-repository ppa:bladerf/bladerf

sudo apt-get update

sudo apt-get install bladerf

sudo apt-get install libbladerf-dev


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





https://github.com/Nuand/bladeRF/tree/master/host/utilities/bladeRF-cli



## Other Links

https://github.com/Nuand/bladeRF/wiki/Getting-Started:-Verifying-Basic-Device-Operation

https://github.com/Nuand/bladeRF/wiki/Gnuradio-OsmoSDR-device-string-options

https://github.com/Nuand/bladeRF/tree/dev-hdl-rx-fifo-fixes

