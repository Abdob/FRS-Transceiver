# FRS-Transceiver PART 2

On [PART1](https://github.com/Abdob/FRS-Transceiver/blob/master/README_PART1.md) we established the GRC bladeRF source block was missing every other buffer.

A quick fix is provided in this part along with the reason why it works. Afterwards a more ellaborate process which led to the solution is described.

## Quick Fix and why it works

for (size_t i = 0; i < noutput_items; ++i) {

	float real = (float)_16icbuf[4*i]/SCALING_FACTOR;
	float imag = (float)_16icbuf[4*i+1]/SCALING_FACTOR;
        out[0][i] = gr_complex(real,imag);

	real = (float)_16icbuf[4*i+2]/SCALING_FACTOR;
	imag = (float)_16icbuf[4*i+3]/SCALING_FACTOR;
        out[1][i] = gr_complex(real,imag);


}
