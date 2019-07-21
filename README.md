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

View your radio's configuration and set frequency and sample rate to match the trasnmitter.

print

set frequency 2100000000

set samplerate 2000000

Capture samples from your transmitting radio:

rx config file=samples.csv format=csv n=4M

rx start

https://github.com/Nuand/bladeRF/tree/master/host/utilities/bladeRF-cli



## Other Links

https://github.com/Nuand/bladeRF/wiki/Getting-Started:-Verifying-Basic-Device-Operation

https://github.com/Nuand/bladeRF/wiki/Gnuradio-OsmoSDR-device-string-options

https://github.com/Nuand/bladeRF/tree/dev-hdl-rx-fifo-fixes

