/* -*- c++ -*- */

#define BLADERF_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "bladerf_swig_doc.i"

%{
#include "bladerf/single_rx.h"
%}


%include "bladerf/single_rx.h"
GR_SWIG_BLOCK_MAGIC2(bladerf, single_rx);
