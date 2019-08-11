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

On Matlab Right Click on the produced files single.csv and dual.csv file and use the Import Data tool. You will get table data named 'single'and 'dual'.

The dual.csv file containing the samples are produced and ported to Matlab, this time you will see the table data containing four columns as opposed to two as before. This is due to having two receive ports each containing inphase and quadrature component totaling to four total components. We are ploting channel 1 output in Matlab again:

Run Matlab script 'ExamineAlternating.m'

GRC Results:

![GitHub Logo](/Diagrams/GRC_Single_RX_Windows.jpg)

![GitHub Logo](/Diagrams/GRC_dual_RX1_Windows.jpg)

![GitHub Logo](/Diagrams/GRC_dual_RX2_Windows.jpg)

BladeRF-cli Results:

![GitHub Logo](/Diagrams/CLI_Single_RX_Windows.jpg)

![GitHub Logo](/Diagrams/CLI_dual_RX1_Windows.jpg)

![GitHub Logo](/Diagrams/CLI_dual_RX2_Windows.jpg)


## If libbladerf was built from source uninstall it and reinstall using apt-get

cd host/build

sudo make uninstall 

Make sure it is uninstalled:

sudo find / -name "libbladeRF.so"

delete any libbladeRF.so library files found.

Install from new:

sudo add-apt-repository ppa:bladerf/bladerf

sudo apt-get update

sudo apt-get install bladerf

sudo apt-get install libbladerf-dev

## Examining single and dual rx in Linux

Using the bladeRF-cli and GRC programs, gather the received data the same way done in the 'Examining single and dual rx in Windows' section.

GRC Results:

![GitHub Logo](/Diagrams/GRC_Single_RX_Linux.jpg)

![GitHub Logo](/Diagrams/GRC_dual_RX1_Linux.jpg)

![GitHub Logo](/Diagrams/GRC_dual_RX2_Linux.jpg)

BladeRF-cli Results:

![GitHub Logo](/Diagrams/CLI_Single_RX_Linux.jpg)

![GitHub Logo](/Diagrams/CLI_dual_RX1_Linux.jpg)

![GitHub Logo](/Diagrams/CLI_dual_RX2_Linux.jpg)

We conclude that bladeRF-cli program works as expected in dual rx mode when libbladeRF is install using apt-get. Now we examine how bladeRF-cli behaves in dual rx mode when building from source.


## Building libbladerf from source (testing bladerf-cli on the apt-get version passed, now we test it when building from source)

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

https://github.com/Nuand/bladeRF/tree/master/host/utilities/bladeRF-cli

## Including libbladeRF built from source and fine and coarse reception to testing:

The bladeRF receiver is tested under 8 total combinations of 3 conditons:

1- libbladeRF is installed by source vs installed using apt-get.

2- Receiver is in single RX vs dual RX mode.

3 - Application is GRC vs CLI.

The combinations of the conditions above are listed from 1-8 as shown on the table:

![GitHub Logo](/Diagrams/intial_setup_table_results.png)

The bladeRF receiver is tested with two waveforms: the FRS waveform and the alternating waveform. The FRS waveform is a high frequency waveform which will test the receivers fine-recovery whereas the alternating waveform is set to a low frequency and will test the receivers coarse-recovery. In order for the test to pass it must pass both the coarse and fine test.

It is found where the tests are passed the tested data works when passed to the FRS GRC application, whereas when the test fails it does not work with FRS GRC application. The CSV files are converted to DAT files in Matlab and subsequently passed to the GRC application with a File source to confirm the tests.

1 - GRC - Single RX Mode - apt-get installation (Pass)

![GitHub Logo](/Diagrams/1.jpg)

2 - GRC - Single RX Mode - installation by source (Pass)

![GitHub Logo](/Diagrams/2.jpg)

3 - GRC - Dual RX Mode - apt-get installation (Failed - Coarse Recovery Problem)

![GitHub Logo](/Diagrams/3.jpg)

4 - GRC - Dual RX Mode - installation by source (Failed - Coarse Recovery Problem)

![GitHub Logo](/Diagrams/4.jpg)

5 - CLI - Single RX Mode - apt-get installation (Passed)

![GitHub Logo](/Diagrams/5.jpg)

6 - CLI - Single RX Mode - installation by source (Failed - Fine Recovery Problem)

![GitHub Logo](/Diagrams/6.jpg)

7 - CLI - Dual RX Mode - apt-get installation (Passed)

![GitHub Logo](/Diagrams/7.jpg)

8 - CLI - Dual RX Mode - installation by source (Failed - Fine Recovery Problem)

![GitHub Logo](/Diagrams/8.jpg)

# Initial Setup Conclusions:

Testing the Linux platform we see bladeRF reciever works in single and dual mode just like the windows platform with CLI program and when libbladeRF is installed through apt-get.  Both linux and windows are experiencing issues with dual recieving on GRC.

Conclusion 1: There is a coarse-receiving issue with gr-osmosdr block.

When building libbladeRF from source CLI experiences a fine-recieving problem on both single and dual RX modes.

Conclusion 2: There is a coarse-receiving issue with cli program only when libbladeRF (including cli itself) is built from source.

# Examining GRC and CLI configurations

To examine the configurations on GRC and CLI we make modifications on the bladerf source. The interface to the bladeRF radio is defined in the header file libbladeRF.h and the function definitions are implemented in the source file bladerf.c . bladerf.c is modified and place [here](https://github.com/Abdob/FRS-Transceiver/blob/master/cpp/bladerf_modified.c). Print statements are placed in the functions where they will be displayed on the terminal as either cli or grc program is running. Of particular interest is the functions: bladerf_sync_config() and bladerf_sync_rx(). Before the aquisition takes place bladerf_sync_config() is called to set the settings of the receiver, the configurations are displayed during this call. bladerf_sync_rx() is the function which does the aquisition and sets a pointer to the buffer containing the rx data.

There were some configuration differences which were found between GRC and CLI. Most were redundant configurations by GRC which were configuring the receiver to values which were already in their default state. One key difference between the two was the length of the buffer set during acquisition. Although both set the buffer size the same during the call to bladerf_sync_config(), GRC limits the buffer size to 4096. During GRC's call to bladerf_sync_rx() varying buffer size less than 4096 are set and acquired whereas the CLI program acquires buffers with the size as defined. We therefore limit the size of the buffer to 4096 and GRC to always acquire this buffer size. 

num_buffers: 32
buffer_size: 4096
num_transfers: 16
stream_timeout: 1000
bladerf_channel_layout: 2
bladerf_format: 0
Chan 0 Frequency: 2099999998
Chan 0 rational samplerate: 2000000, 0, 1
Chan 0 bandwidth: 6000000
Chan 0 Gain: 54
Chan 0 Gain Mode: 1
Chan 2 Frequency: 2099999998
Chan 2 rational samplerate: 2000000, 0, 1
Chan 2 bandwidth: 6000000
Chan 2 Gain: 60
Chan 2 Gain Mode: 1
rx mux mode: 0
loopback mode: 0
tuning mode: 0




![GitHub Logo](/Diagrams/rx_buffers.jpg)


## Other Links

https://github.com/Nuand/bladeRF/wiki/Getting-Started:-Verifying-Basic-Device-Operation

https://github.com/Nuand/bladeRF/wiki/Gnuradio-OsmoSDR-device-string-options

https://github.com/Nuand/bladeRF/tree/dev-hdl-rx-fifo-fixes

