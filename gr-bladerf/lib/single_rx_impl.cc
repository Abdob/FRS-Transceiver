/* -*- c++ -*- */
/* 
 * Copyright 2019 foci.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */




#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "single_rx_impl.h"


static int init_sync(struct bladerf *dev)
{
    int status;
    /* These items configure the underlying asynch stream used by the sync
     * interface. The "buffer" here refers to those used internally by worker
     * threads, not the user's sample buffers.
     *
     * It is important to remember that TX buffers will not be submitted to
     * the hardware until `buffer_size` samples are provided via the
     * bladerf_sync_tx call.  Similarly, samples will not be available to
     * RX via bladerf_sync_rx() until a block of `buffer_size` samples has been
     * received.
     */
    const unsigned int num_buffers   = 16;
    const unsigned int buffer_size   = 4096; /* Must be a multiple of 1024 */
    const unsigned int num_transfers = 8;
    const unsigned int timeout_ms    = 3500;
    /* Configure both the device's x1 RX and TX channels for use with the
     * synchronous
     * interface. SC16 Q11 samples *without* metadata are used. */
    status = bladerf_sync_config(dev, BLADERF_RX_X2, BLADERF_FORMAT_SC16_Q11,
                                 num_buffers, buffer_size, num_transfers,
                                 timeout_ms);
    if (status != 0) {
        fprintf(stderr, "Failed to configure RX sync interface: %s\n",
                bladerf_strerror(status));
        return status;
    }
    else {
        printf("Succesfully configured interface\n ");
    }


}


/* The RX and TX channels are configured independently for these parameters */
struct channel_config {
    bladerf_channel channel;
    unsigned int frequency;
    unsigned int bandwidth;
    unsigned int samplerate;
    int gain;
};
int configure_channel(struct bladerf *dev, struct channel_config *c)
{
    int status;
    status = bladerf_set_frequency(dev, c->channel, c->frequency);
    if (status != 0) {
        fprintf(stderr, "Failed to set frequency = %u: %s\n", c->frequency,
                bladerf_strerror(status));
        return status;
    }
    status = bladerf_set_sample_rate(dev, c->channel, c->samplerate, NULL);
    if (status != 0) {
        fprintf(stderr, "Failed to set samplerate = %u: %s\n", c->samplerate,
                bladerf_strerror(status));
        return status;
    }
    status = bladerf_set_bandwidth(dev, c->channel, c->bandwidth, NULL);
    if (status != 0) {
        fprintf(stderr, "Failed to set bandwidth = %u: %s\n", c->bandwidth,
                bladerf_strerror(status));
        return status;
    }
    status = bladerf_set_gain(dev, c->channel, c->gain);
    if (status != 0) {
        fprintf(stderr, "Failed to set gain: %s\n", bladerf_strerror(status));
        return status;
    }
    return status;
}


namespace gr {
  namespace bladerf {


    static unsigned int iter = 0;
    unsigned long int t2;
    static unsigned long int t1;
    double time;

    single_rx::sptr
    single_rx::make()
    {
      return gnuradio::get_initial_sptr
        (new single_rx_impl());

    }

    /*
     * The private constructor
     */
    single_rx_impl::single_rx_impl()
      : gr::sync_block("single_rx",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof( gr_complex ))),
    _dev(NULL)
    {
	    set_max_noutput_items(2048);
	    set_output_multiple(2048);
	    int status;
	    struct channel_config config;
	    struct bladerf_devinfo dev_info;
	    /* Initialize the information used to identify the desired device
	     * to all wildcard (i.e., "any device") values */
	    bladerf_init_devinfo(&dev_info);
	    status = bladerf_open_with_devinfo(&_dev, &dev_info);
	    if (status != 0) {
		fprintf(stderr, "Unable to open device: %s\n",
		        bladerf_strerror(status));
	    }
	    /* Set up RX channel parameters */
	    config.channel    = BLADERF_CHANNEL_RX(0);
	    config.frequency  = 2100000000;
	    config.bandwidth  = 6000000;
	    config.samplerate = 2000000;
	    config.gain       = 30;
	    status = configure_channel(_dev, &config);
	    if (status != 0) {
		fprintf(stderr, "Failed to configure RX channel. Exiting.\n");
	    }
            int ret;
    	    bool done         = false;
    	    bool have_tx_data = false;
    	    unsigned long int t1,t2;
    	    double time;
            /* "User" samples buffers and their associated sizes, in units of samples.
            * Recall that one sample = two int16_t values. */
            rx_samples            = NULL;
            int16_t *tx_samples            = NULL;
            const unsigned int samples_len = 4096; /* May be any (reasonable) size */
            /* Allocate a buffer to store received samples in */
            rx_samples = (int16_t*)malloc(samples_len * 2 * 1 * sizeof(int16_t));
    	    if (rx_samples == NULL) {
        	perror("malloc");
    	    }

	    /* Initialize synch interface on RX and TX */
	    status = init_sync(_dev);
	    if (status != 0) {
	    }
	    status = bladerf_enable_module(_dev, BLADERF_CHANNEL_RX(0), true);
	    if (status != 0) {
		fprintf(stderr, "Failed to enable RX: %s\n", bladerf_strerror(status));
	    }
            usleep(2000000);
	    status = bladerf_enable_module(_dev, BLADERF_CHANNEL_RX(1), true);
	    if (status != 0) {
		fprintf(stderr, "Failed to enable RX: %s\n", bladerf_strerror(status));
	    }
    }

    /*
     * Our virtual destructor.
     */
    single_rx_impl::~single_rx_impl()
    {
	int status;
	/* Disable RX, shutting down our underlying RX stream */
	status = bladerf_enable_module(_dev, BLADERF_CHANNEL_RX(0), false);
	if (status != 0) {
	fprintf(stderr, "Failed to disable RX: %s\n", bladerf_strerror(status));
	}

	/* Disable RX, shutting down our underlying RX stream */
        
	/*status = bladerf_enable_module(_dev, BLADERF_CHANNEL_RX(1), false);
	if (status != 0) {
	fprintf(stderr, "Failed to disable RX: %s\n", bladerf_strerror(status));
	}*/

	/* Free up our resources */
	free(rx_samples);
    }

    int
    single_rx_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
        int status;
	const unsigned int samples_len = 4096; /* May be any (reasonable) size */
        unsigned int timeout_ms = 1000;
	iter++;
        gr_complex *out = ( gr_complex *) output_items[0];
	status = bladerf_sync_rx(_dev, rx_samples, samples_len, NULL, timeout_ms);
	t2 = clock();
	time = (float)(t2 - t1)/CLOCKS_PER_SEC*1000;
      	if(iter % 1024 == 0){
	   printf("iter: %u time: %fms: spb: %u timeout_ms: noutput_items: %u\n", iter, time, samples_len, noutput_items);
	}

	if(noutput_items > 3000)
          printf(",");
  	t1 = clock();
      // Do <+signal processing+>
       for(int i = 0; i < noutput_items ; i++)
        {
        	// ML decoder, determine the minimum distance from all constellation points
		float real = (float)rx_samples[4*i]/2048;
		float imag = (float)rx_samples[4*i+1]/2048;
      	if(iter % 1024*100 == 0){

		if(real > 4 | imag > 4)
		    printf("iter: %u , i: %u\n" , iter,i );

}
                out[i] = gr_complex(real,imag);

        }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace bladerf */
} /* namespace gr */

