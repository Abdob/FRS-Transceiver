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

## Notes

When building gr-osmosdr rx/tx source block, cmake looks for the bladerf library: libbladeRF.so

Found libbladeRF: /usr/include, /usr/lib/x86_64-linux-gnu/libbladeRF.so

## Links
https://github.com/Nuand/bladeRF/wiki/Getting-Started%3A-Linux

https://github.com/Nuand/bladeRF/wiki/Getting-Started:-Verifying-Basic-Device-Operation

https://github.com/Nuand/bladeRF/wiki/Gnuradio-OsmoSDR-device-string-options

https://github.com/Nuand/bladeRF/tree/dev-hdl-rx-fifo-fixes

https://github.com/osmocom/gr-osmosdr
