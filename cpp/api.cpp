#include <iostream>
#include <libbladeRF.h>
#include <time.h>
#include <unistd.h>
/* Save to a file, e.g. boilerplate.c, and then compile:
 * $ gcc boilerplate.c -o libbladeRF_example_boilerplate -lbladeRF
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using namespace std;
//g++ rx.cpp -lbladeRF -L/usr/local/lib


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
    const unsigned int buffer_size   = 8192; /* Must be a multiple of 1024 */
    const unsigned int num_transfers = 8;
    const unsigned int timeout_ms    = 3500;
    /* Configure both the device's x1 RX and TX channels for use with the
     * synchronous
     * interface. SC16 Q11 samples *without* metadata are used. */
    status = bladerf_sync_config(dev, BLADERF_RX_X1, BLADERF_FORMAT_SC16_Q11,
                                 num_buffers, buffer_size, num_transfers,
                                 timeout_ms);
    if (status != 0) {
        fprintf(stderr, "Failed to configure RX sync interface: %s\n",
                bladerf_strerror(status));
        return status;
    }
    status = bladerf_sync_config(dev, BLADERF_TX_X1, BLADERF_FORMAT_SC16_Q11,
                                 num_buffers, buffer_size, num_transfers,
                                 timeout_ms);
    if (status != 0) {
        fprintf(stderr, "Failed to configure TX sync interface: %s\n",
                bladerf_strerror(status));
    }
    return status;
}
int sync_rx_example(struct bladerf *dev)
{
    int status, ret;
    bool done         = false;
    bool have_tx_data = false;
    /* "User" samples buffers and their associated sizes, in units of samples.
     * Recall that one sample = two int16_t values. */
    int16_t *rx_samples            = NULL;
    int16_t *tx_samples            = NULL;
    const unsigned int samples_len = 10000; /* May be any (reasonable) size */
    /* Allocate a buffer to store received samples in */
    rx_samples = (int16_t*)malloc(samples_len * 2 * 1 * sizeof(int16_t));
    if (rx_samples == NULL) {
        perror("malloc");
        return BLADERF_ERR_MEM;
    }
    /* Allocate a buffer to prepare transmit data in */
    tx_samples = (int16_t*)malloc(samples_len * 2 * 1 * sizeof(int16_t));
    if (tx_samples == NULL) {
        perror("malloc");
        free(rx_samples);
        return BLADERF_ERR_MEM;
    }
    /* Initialize synch interface on RX and TX */
    status = init_sync(dev);
    if (status != 0) {
        goto out;
    }
    status = bladerf_enable_module(dev, BLADERF_RX, true);
    if (status != 0) {
        fprintf(stderr, "Failed to enable RX: %s\n", bladerf_strerror(status));
        goto out;
    }
    status = bladerf_enable_module(dev, BLADERF_TX, true);
    if (status != 0) {
        fprintf(stderr, "Failed to enable TX: %s\n", bladerf_strerror(status));
        goto out;
    }
    for(int i=0; i< 5; i++) {
        /* Receive samples */
        status = bladerf_sync_rx(dev, rx_samples, samples_len, NULL, 5000);
        if (status == 0) {
	    printf("bladerf bladerf_sync_rx pass\n");
        } else {
            fprintf(stderr, "Failed to RX samples: %s\n",
                    bladerf_strerror(status));
        }
    }
    if (status == 0) {
        /* Wait a few seconds for any remaining TX samples to finish
         * reaching the RF front-end */
        usleep(2000000);
    }
out:
    ret = status;
    /* Disable RX, shutting down our underlying RX stream */
    status = bladerf_enable_module(dev, BLADERF_RX, false);
    if (status != 0) {
        fprintf(stderr, "Failed to disable RX: %s\n", bladerf_strerror(status));
    }
    /* Disable TX, shutting down our underlying TX stream */
    status = bladerf_enable_module(dev, BLADERF_TX, false);
    if (status != 0) {
        fprintf(stderr, "Failed to disable TX: %s\n", bladerf_strerror(status));
    }
    /* Free up our resources */
    free(rx_samples);
    free(tx_samples);
    return ret;
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
/* Usage:
 *   libbladeRF_example_boilerplate [serial #]
 *
 * If a serial number is supplied, the program will attempt to open the
 * device with the provided serial number.
 *
 * Otherwise, the first available device will be used.
 */
int main(int argc, char *argv[])
{
    int status;
    struct channel_config config;
    struct bladerf *dev = NULL;
    struct bladerf_devinfo dev_info;
    /* Initialize the information used to identify the desired device
     * to all wildcard (i.e., "any device") values */
    bladerf_init_devinfo(&dev_info);
    /* Request a device with the provided serial number.
     * Invalid strings should simply fail to match a device. */
    if (argc >= 2) {
        strncpy(dev_info.serial, argv[1], sizeof(dev_info.serial) - 1);
    }
    status = bladerf_open_with_devinfo(&dev, &dev_info);
    if (status != 0) {
        fprintf(stderr, "Unable to open device: %s\n",
                bladerf_strerror(status));
        return 1;
    }
    /* Set up RX channel parameters */
    config.channel    = BLADERF_CHANNEL_RX(0);
    config.frequency  = 2100000000;
    config.bandwidth  = 6000000;
    config.samplerate = 2000000;
    config.gain       = 39;
    status = configure_channel(dev, &config);
    if (status != 0) {
        fprintf(stderr, "Failed to configure RX channel. Exiting.\n");
        goto out;
    }
    /* Set up TX channel parameters */
    config.channel    = BLADERF_CHANNEL_TX(0);
    config.frequency  = 2100000000;
    config.bandwidth  = 6000000;
    config.samplerate = 2000000;
    config.gain       = -14;
    status = configure_channel(dev, &config);
    if (status != 0) {
        fprintf(stderr, "Failed to configure TX channel. Exiting.\n");
        goto out;
    }
init_sync(dev);
sync_rx_example(dev);
    /* Application code goes here.
     *
     * Don't forget to call bladerf_enable_module() before attempting to
     * transmit or receive samples!
     */
    printf("Hello world\n");
out:
    bladerf_close(dev);
    return status;
}


