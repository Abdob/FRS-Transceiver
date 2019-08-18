# FRS-Transceiver PART 2

On [PART1](https://github.com/Abdob/FRS-Transceiver/blob/master/README_PART1.md) we established the GRC bladeRF source block was missing every other buffer.

A quick fix is provided in this part along with the reason why it works. Afterwards a more ellaborate process which led to the solution is described.

## Quick Fix and why it works

1st fix - allocate memory needed for Dual RX

  _16icbuf = reinterpret_cast<int16_t *>(volk_malloc(4*_samples_per_buffer*sizeof(int16_t), alignment));
  _32fcbuf = reinterpret_cast<gr_complex *>(volk_malloc(2*_samples_per_buffer*sizeof(gr_complex), alignment));

We are doubling the memory allocation. Although _samples_per_buffer is the number of samples we obtain for each RX channel. The samples between RX1 and RX2 are interleaved and share the same buffer, the actual number of samples per buffer is double.

2nd fix - Acquire twice as many samples for dual rx mode

  // grab samples into temp buffer
  if(nstreams > 1)
    status = bladerf_sync_rx(_dev.get(), static_cast<void *>(_16icbuf), 2 * noutput_items, meta_ptr, _stream_timeout);
  else
    status = bladerf_sync_rx(_dev.get(), static_cast<void *>(_16icbuf), noutput_items, meta_ptr, _stream_timeout);
    
As mentioned, the samples are interleaved between the channels, therefore we double the number of samples in dual rx mode to preserve the number of samples per channel.

3rd fix - Deinterleave all the samples

  if (nstreams > 1) {
    // we need to deinterleave the multiplex as we copy
    gr_complex const *deint_in = _32fcbuf;
    for (size_t i = 0; i < noutput_items; ++i) {
      for (size_t n = 0; n < nstreams; ++n) {
        memcpy(out[n]++, deint_in++, sizeof(gr_complex));
      }
    }
  } else {
    // no deinterleaving to do: simply copy everything
    memcpy(out[0], _32fcbuf, sizeof(gr_complex) * noutput_items);
  }
  
Prior to this fix half the samples we being deinterleaved so we change the outter for loop to loop noutput_items times.


# Why this wasn't working

The osmosdr community may have not updated the bladerf block after the library changed. Perhaps both channels had separated buffers when osmosdr was developed for bladeRF.




