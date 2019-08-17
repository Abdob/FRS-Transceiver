#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib
export PATH=/home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib:$PATH
export LD_LIBRARY_PATH=/home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-bladerf 
