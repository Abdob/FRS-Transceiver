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

![GitHub Logo](/Diagrams/singleRX_1.jpg)

![GitHub Logo](/Diagrams/dualRX_1.jpg)

## Links
https://github.com/Nuand/bladeRF/wiki/Getting-Started:-Verifying-Basic-Device-Operation

https://github.com/Nuand/bladeRF/wiki/Gnuradio-OsmoSDR-device-string-options

