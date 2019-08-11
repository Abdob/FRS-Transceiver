/*
 * This file is part of the bladeRF project:
 *   http://www.github.com/nuand/bladeRF
 *
 * Copyright (C) 2013-2016 Nuand LLC
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libbladeRF.h>

#include "log.h"
#include "rel_assert.h"
#define LOGGER_ID_STRING
#include "logger_entry.h"
#include "logger_id.h"

#include "backend/backend.h"
#include "backend/usb/usb.h"
#include "board/board.h"
#include "driver/fx3_fw.h"
#include "streaming/async.h"
#include "version.h"

#include "expansion/xb100.h"
#include "expansion/xb200.h"
#include "expansion/xb300.h"

#include "devinfo.h"
#include "helpers/configfile.h"
#include "helpers/file.h"
#include "helpers/interleave.h"


/******************************************************************************/
/* Open / Close */
/******************************************************************************/

/* dev path becomes device specifier string (osmosdr-like) */
int bladerf_open(struct bladerf **dev, const char *dev_id)
{
    printf("bladerf_open .. .. ..\n");
    struct bladerf_devinfo devinfo;
    int status;

    *dev = NULL;

    /* Populate dev-info from string */
    status = str2devinfo(dev_id, &devinfo);
    if (!status) {
        status = bladerf_open_with_devinfo(dev, &devinfo);
    }

    return status;
}

int bladerf_open_with_devinfo(struct bladerf **opened_device,
                              struct bladerf_devinfo *devinfo)
{
    printf("bladerf_open_with_devinfo .. .. ..\n");
    struct bladerf *dev;
    struct bladerf_devinfo any_device;
    unsigned int i;
    int status;

    if (devinfo == NULL) {
        bladerf_init_devinfo(&any_device);
        devinfo = &any_device;
    }

    *opened_device = NULL;

    dev = calloc(1, sizeof(struct bladerf));
    if (dev == NULL) {
        return BLADERF_ERR_MEM;
    }

    /* Open backend */
    status = backend_open(dev, devinfo);
    if (status != 0) {
        free(dev);
        return status;
    }

    /* Find matching board */
    for (i = 0; i < bladerf_boards_len; i++) {
        if (bladerf_boards[i]->matches(dev)) {
            dev->board = bladerf_boards[i];
            break;
        }
    }
    /* If no matching board was found */
    if (i == bladerf_boards_len) {
        dev->backend->close(dev);
        free(dev);
        return BLADERF_ERR_NODEV;
    }

    MUTEX_INIT(&dev->lock);

    /* Open board */
    status = dev->board->open(dev, devinfo);

    if (status < 0) {
        bladerf_close(dev);
        return status;
    }

    /* Load configuration file */
    status = config_load_options_file(dev);

    if (status < 0) {
        bladerf_close(dev);
        return status;
    }

    *opened_device = dev;

    return 0;
}

int bladerf_get_devinfo(struct bladerf *dev, struct bladerf_devinfo *info)
{
    printf("bladerf_get_devinfo .. .. ..\n");
    if (dev) {
        MUTEX_LOCK(&dev->lock);
        memcpy(info, &dev->ident, sizeof(struct bladerf_devinfo));
        MUTEX_UNLOCK(&dev->lock);
        return 0;
    } else {
        return BLADERF_ERR_INVAL;
    }
}

void bladerf_close(struct bladerf *dev)
{
    printf("bladerf_close .. .. ..\n");
    if (dev) {
        MUTEX_LOCK(&dev->lock);

        dev->board->close(dev);

        if (dev->backend) {
            dev->backend->close(dev);
        }

        MUTEX_UNLOCK(&dev->lock);

        free(dev);
    }
}

/******************************************************************************/
/* FX3 Firmware (common to bladerf1 and bladerf2) */
/******************************************************************************/

int bladerf_jump_to_bootloader(struct bladerf *dev)
{
    printf("bladerf_jump_to_bootloader .. .. ..\n");
    int status;

    if (!dev->backend->jump_to_bootloader) {
        return BLADERF_ERR_UNSUPPORTED;
    }

    MUTEX_LOCK(&dev->lock);

    status = dev->backend->jump_to_bootloader(dev);

    MUTEX_UNLOCK(&dev->lock);

    return status;
}

int bladerf_get_bootloader_list(struct bladerf_devinfo **devices)
{   
    printf("bladerf_get_bootloader_list.. .. ..\n");
    return probe(BACKEND_PROBE_FX3_BOOTLOADER, devices);
}

int bladerf_load_fw_from_bootloader(const char *device_identifier,
                                    bladerf_backend backend,
                                    uint8_t bus,
                                    uint8_t addr,
                                    const char *file)
{
    printf("bladerf_load_fw_from_bootloader .. .. ..\n");
    int status;
    uint8_t *buf;
    size_t buf_len;
    struct fx3_firmware *fw = NULL;
    struct bladerf_devinfo devinfo;

    if (device_identifier == NULL) {
        bladerf_init_devinfo(&devinfo);
        devinfo.backend  = backend;
        devinfo.usb_bus  = bus;
        devinfo.usb_addr = addr;
    } else {
        status = str2devinfo(device_identifier, &devinfo);
        if (status != 0) {
            return status;
        }
    }

    status = file_read_buffer(file, &buf, &buf_len);
    if (status != 0) {
        return status;
    }

    status = fx3_fw_parse(&fw, buf, buf_len);
    free(buf);
    if (status != 0) {
        return status;
    }

    assert(fw != NULL);

    status = backend_load_fw_from_bootloader(devinfo.backend, devinfo.usb_bus,
                                             devinfo.usb_addr, fw);

    fx3_fw_free(fw);

    return status;
}

/* FIXME presumes bladerf1 capability bits */
#include "board/bladerf1/capabilities.h"

int bladerf_get_fw_log(struct bladerf *dev, const char *filename)
{
    printf("bladerf_get_fw_log .. .. ..\n");
    int status;
    FILE *f = NULL;
    logger_entry e;

    MUTEX_LOCK(&dev->lock);

    if (!have_cap(dev->board->get_capabilities(dev),
                  BLADERF_CAP_READ_FW_LOG_ENTRY)) {
        struct bladerf_version fw_version;

        if (dev->board->get_fw_version(dev, &fw_version) == 0) {
            log_debug("FX3 FW v%s does not support log retrieval.\n",
                      fw_version.describe);
        }

        status = BLADERF_ERR_UNSUPPORTED;
        goto error;
    }

    if (filename != NULL) {
        f = fopen(filename, "w");
        if (f == NULL) {
            switch (errno) {
                case ENOENT:
                    status = BLADERF_ERR_NO_FILE;
                    break;
                case EACCES:
                    status = BLADERF_ERR_PERMISSION;
                    break;
                default:
                    status = BLADERF_ERR_IO;
                    break;
            }
            goto error;
        }
    } else {
        f = stdout;
    }

    do {
        status = dev->backend->read_fw_log(dev, &e);
        if (status != 0) {
            log_debug("Failed to read FW log: %s\n", bladerf_strerror(status));
            goto error;
        }

        if (e == LOG_ERR) {
            fprintf(f, "<Unexpected error>,,\n");
        } else if (e != LOG_EOF) {
            uint8_t file_id;
            uint16_t line;
            uint16_t data;
            const char *src_file;

            logger_entry_unpack(e, &file_id, &line, &data);
            src_file = logger_id_string(file_id);

            fprintf(f, "%s, %u, 0x%04x\n", src_file, line, data);
        }
    } while (e != LOG_EOF && e != LOG_ERR);

error:
    MUTEX_UNLOCK(&dev->lock);

    if (f != NULL && f != stdout) {
        fclose(f);
    }

    return status;
}

/******************************************************************************/
/* Properties */
/******************************************************************************/

bladerf_dev_speed bladerf_device_speed(struct bladerf *dev)
{
    printf("bladerf_device_speed .. .. ..\n");
    bladerf_dev_speed speed;
    MUTEX_LOCK(&dev->lock);

    speed = dev->board->device_speed(dev);

    MUTEX_UNLOCK(&dev->lock);
    return speed;
}

int bladerf_get_serial(struct bladerf *dev, char *serial)
{
    printf("bladerf_get_serial .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    /** TODO: add deprecation warning */

    status = dev->board->get_serial(dev, serial);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_serial_struct(struct bladerf *dev,
                              struct bladerf_serial *serial)
{
    printf("bladerf_get_serial_struct .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    char serialstr[sizeof(serial->serial)];

    status = dev->board->get_serial(dev, serialstr);

    if (status >= 0) {
        strncpy(serial->serial, serialstr, sizeof(serial->serial));
        serial->serial[sizeof(serial->serial)-1] = '\0';
    }

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_fpga_size(struct bladerf *dev, bladerf_fpga_size *size)
{
    printf("bladerf_get_fpga_size .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_fpga_size(dev, size);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_fpga_bytes(struct bladerf *dev, size_t *size)
{
    printf("bladerf_get_fpga_bytes .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_fpga_bytes(dev, size);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_flash_size(struct bladerf *dev, uint32_t *size, bool *is_guess)
{
    printf("bladerf_get_flash_size .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_flash_size(dev, size, is_guess);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_is_fpga_configured(struct bladerf *dev)
{
    printf("bladerf_is_fpga_configured .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->is_fpga_configured(dev);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_fpga_source(struct bladerf *dev, bladerf_fpga_source *source)
{
    printf("bladerf_get_fpga_source .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_fpga_source(dev, source);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

const char *bladerf_get_board_name(struct bladerf *dev)
{
    printf("bladerf_get_board_name .. .. ..\n");
    return dev->board->name;
}

size_t bladerf_get_channel_count(struct bladerf *dev, bladerf_direction dir)
{
    //printf("bladerf_get_channel_count .. .. ..\n");
    return dev->board->get_channel_count(dev, dir);
}

/******************************************************************************/
/* Versions */
/******************************************************************************/

int bladerf_fpga_version(struct bladerf *dev, struct bladerf_version *version)
{
    printf("bladerf_fpga_version .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_fpga_version(dev, version);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_fw_version(struct bladerf *dev, struct bladerf_version *version)
{
    printf("bladerf_fw_version .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_fw_version(dev, version);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

void bladerf_version(struct bladerf_version *version)
{
    printf("bladerf_version .. .. ..\n");
/* Sanity checks for version reporting mismatches */
#ifndef LIBBLADERF_API_VERSION
#error LIBBLADERF_API_VERSION is missing
#endif
#if LIBBLADERF_VERSION_MAJOR != ((LIBBLADERF_API_VERSION >> 24) & 0xff)
#error LIBBLADERF_API_VERSION: Major version mismatch
#endif
#if LIBBLADERF_VERSION_MINOR != ((LIBBLADERF_API_VERSION >> 16) & 0xff)
#error LIBBLADERF_API_VERSION: Minor version mismatch
#endif
#if LIBBLADERF_VERSION_PATCH != ((LIBBLADERF_API_VERSION >> 8) & 0xff)
#error LIBBLADERF_API_VERSION: Patch version mismatch
#endif
    version->major    = LIBBLADERF_VERSION_MAJOR;
    version->minor    = LIBBLADERF_VERSION_MINOR;
    version->patch    = LIBBLADERF_VERSION_PATCH;
    version->describe = LIBBLADERF_VERSION;
}

/******************************************************************************/
/* Enable/disable */
/******************************************************************************/

int bladerf_enable_module(struct bladerf *dev, bladerf_channel ch, bool enable)
{
    //printf("bladerf_enable_module .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->enable_module(dev, ch, enable);
    //printf("Module ch %u enabled: %u\n", ch,

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Gain */
/******************************************************************************/

int bladerf_set_gain(struct bladerf *dev, bladerf_channel ch, int gain)
{
    //printf("bladerf_set_gain .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_gain(dev, ch, gain);


    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_gain(struct bladerf *dev, bladerf_channel ch, int *gain)
{
    //printf("bladerf_get_gain .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_gain(dev, ch, gain);
//    printf("Got gain: %d\n" , *gain);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_set_gain_mode(struct bladerf *dev,
                          bladerf_channel ch,
                          bladerf_gain_mode mode)
{
    //printf("bladerf_set_gain_mode .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_gain_mode(dev, ch, mode);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_gain_mode(struct bladerf *dev,
                          bladerf_channel ch,
                          bladerf_gain_mode *mode)
{
    //printf("bladerf_get_gain_mode .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_gain_mode(dev, ch, mode);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_gain_modes(struct bladerf *dev,
                           bladerf_channel ch,
                           struct bladerf_gain_modes const **modes)
{
    printf("bladerf_get_gain_modes .. .. ..\n");
    return dev->board->get_gain_modes(dev, ch, modes);
}

int bladerf_get_gain_range(struct bladerf *dev,
                           bladerf_channel ch,
                           struct bladerf_range const **range)
{
    printf("bladerf_get_gain_modes .. .. ..\n");
    return dev->board->get_gain_range(dev, ch, range);
}

int bladerf_set_gain_stage(struct bladerf *dev,
                           bladerf_channel ch,
                           const char *stage,
                           bladerf_gain gain)
{
    printf("bladerf_set_gain_stage .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_gain_stage(dev, ch, stage, gain);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_gain_stage(struct bladerf *dev,
                           bladerf_channel ch,
                           const char *stage,
                           bladerf_gain *gain)
{
    printf("bladerf_get_gain_stage .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_gain_stage(dev, ch, stage, gain);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_gain_stage_range(struct bladerf *dev,
                                 bladerf_channel ch,
                                 const char *stage,
                                 struct bladerf_range const **range)
{
    printf("bladerf_get_gain_stage_range .. .. ..\n");
    return dev->board->get_gain_stage_range(dev, ch, stage, range);
}

int bladerf_get_gain_stages(struct bladerf *dev,
                            bladerf_channel ch,
                            const char **stages,
                            size_t count)
{
    printf("bladerf_get_gain_stages .. .. ..\n");
    return dev->board->get_gain_stages(dev, ch, stages, count);
}

/******************************************************************************/
/* Sample Rate */
/******************************************************************************/

int bladerf_set_sample_rate(struct bladerf *dev,
                            bladerf_channel ch,
                            bladerf_sample_rate rate,
                            bladerf_sample_rate *actual)
{
    //printf("Chan %u bladerf_set_sample_rate .. .. ..\n", ch);
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_sample_rate(dev, ch, rate, actual);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_sample_rate(struct bladerf *dev,
                            bladerf_channel ch,
                            bladerf_sample_rate *rate)
{
    //printf("bladerf_get_sample_rate .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_sample_rate(dev, ch, rate);
    //printf("Chan %u Got samplerate: %u\n", ch, *rate);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_sample_rate_range(struct bladerf *dev,
                                  bladerf_channel ch,
                                  const struct bladerf_range **range)
{
    printf("bladerf_get_sample_rate_range .. .. ..\n");
    return dev->board->get_sample_rate_range(dev, ch, range);
}

int bladerf_get_rational_sample_rate(struct bladerf *dev,
                                     bladerf_channel ch,
                                     struct bladerf_rational_rate *rate)
{
    //printf("Chan %u bladerf_get_rational_sample_rate .. .. ..\n",ch);
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_rational_sample_rate(dev, ch, rate);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_set_rational_sample_rate(struct bladerf *dev,
                                     bladerf_channel ch,
                                     struct bladerf_rational_rate *rate,
                                     struct bladerf_rational_rate *actual)
{
    //printf("bladerf_set_rational_sample_rate .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_rational_sample_rate(dev, ch, rate, actual);
    //printf("Chan %u rational samplerate: %lu, %lu, %lu\n", ch, rate->integer, rate->num, rate->den);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}



/******************************************************************************/
/* Bandwidth */
/******************************************************************************/

int bladerf_set_bandwidth(struct bladerf *dev,
                          bladerf_channel ch,
                          bladerf_bandwidth bandwidth,
                          bladerf_bandwidth *actual)
{
    //printf("bladerf_set_bandwidth .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_bandwidth(dev, ch, bandwidth, actual);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}




int bladerf_get_bandwidth(struct bladerf *dev,
                          bladerf_channel ch,
                          bladerf_bandwidth *bandwidth)
{
    //printf("bladerf_get_bandwidth .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_bandwidth(dev, ch, bandwidth);
    //printf("Chan %u bandwidth: %d\n", ch, *bandwidth);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_bandwidth_range(struct bladerf *dev,
                                bladerf_channel ch,
                                const struct bladerf_range **range)
{
    printf("Chan %u bladerf_get_bandwidth_range .. .. ..\n",ch);
    return dev->board->get_bandwidth_range(dev, ch, range);
}

/******************************************************************************/
/* Frequency */
/******************************************************************************/

int bladerf_set_frequency(struct bladerf *dev,
                          bladerf_channel ch,
                          bladerf_frequency frequency)
{
    //printf("Chan %u bladerf_set_frequency .. .. ..\n",ch);
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_frequency(dev, ch, frequency);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_frequency(struct bladerf *dev,
                          bladerf_channel ch,
                          bladerf_frequency *frequency)
{
    //printf("bladerf_get_frequency .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_frequency(dev, ch, frequency);
    //printf("Chan %u Frequency: %lu\n" ,ch, *frequency);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_frequency_range(struct bladerf *dev,
                                bladerf_channel ch,
                                const struct bladerf_range **range)
{
    //printf("Chan %u bladerf_get_frequency_range .. .. ..\n", ch);
    return dev->board->get_frequency_range(dev, ch, range);
}

int bladerf_select_band(struct bladerf *dev,
                        bladerf_channel ch,
                        bladerf_frequency frequency)
{
    printf("bladerf_select_band .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->select_band(dev, ch, frequency);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* RF Ports*/
/******************************************************************************/

int bladerf_set_rf_port(struct bladerf *dev,
                        bladerf_channel ch,
                        const char *port)
{
    printf("bladerf_set_rf_port .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_rf_port(dev, ch, port);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_rf_port(struct bladerf *dev,
                        bladerf_channel ch,
                        const char **port)
{
    printf("bladerf_get_rf_port .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_rf_port(dev, ch, port);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_rf_ports(struct bladerf *dev,
                         bladerf_channel ch,
                         const char **ports,
                         unsigned int count)
{
    printf("bladerf_get_rf_ports .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_rf_ports(dev, ch, ports, count);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Scheduled Tuning */
/******************************************************************************/

int bladerf_get_quick_tune(struct bladerf *dev,
                           bladerf_channel ch,
                           struct bladerf_quick_tune *quick_tune)
{
    printf("bladerf_get_quick_tune .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_quick_tune(dev, ch, quick_tune);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_schedule_retune(struct bladerf *dev,
                            bladerf_channel ch,
                            bladerf_timestamp timestamp,
                            bladerf_frequency frequency,
                            struct bladerf_quick_tune *quick_tune)

{
    printf("bladerf_schedule_retune .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status =
        dev->board->schedule_retune(dev, ch, timestamp, frequency, quick_tune);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_cancel_scheduled_retunes(struct bladerf *dev, bladerf_channel ch)
{
    printf("bladerf_cancel_scheduled_retunes .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->cancel_scheduled_retunes(dev, ch);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* DC/Phase/Gain Correction */
/******************************************************************************/

int bladerf_get_correction(struct bladerf *dev,
                           bladerf_channel ch,
                           bladerf_correction corr,
                           bladerf_correction_value *value)
{
    printf("bladerf_get_correction .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_correction(dev, ch, corr, value);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_set_correction(struct bladerf *dev,
                           bladerf_channel ch,
                           bladerf_correction corr,
                           bladerf_correction_value value)
{
    printf("bladerf_set_correction .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_correction(dev, ch, corr, value);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Trigger */
/******************************************************************************/

int bladerf_trigger_init(struct bladerf *dev,
                         bladerf_channel ch,
                         bladerf_trigger_signal signal,
                         struct bladerf_trigger *trigger)
{
    printf("bladerf_trigger_init .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->trigger_init(dev, ch, signal, trigger);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_trigger_arm(struct bladerf *dev,
                        const struct bladerf_trigger *trigger,
                        bool arm,
                        uint64_t resv1,
                        uint64_t resv2)
{
    printf("bladerf_trigger_arm .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->trigger_arm(dev, trigger, arm, resv1, resv2);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_trigger_fire(struct bladerf *dev,
                         const struct bladerf_trigger *trigger)
{
    printf("bladerf_trigger_fire .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->trigger_fire(dev, trigger);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_trigger_state(struct bladerf *dev,
                          const struct bladerf_trigger *trigger,
                          bool *is_armed,
                          bool *has_fired,
                          bool *fire_requested,
                          uint64_t *reserved1,
                          uint64_t *reserved2)
{
    printf("bladerf_trigger_state .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->trigger_state(dev, trigger, is_armed, has_fired,
                                       fire_requested, reserved1, reserved2);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Streaming */
/******************************************************************************/

int bladerf_init_stream(struct bladerf_stream **stream,
                        struct bladerf *dev,
                        bladerf_stream_cb callback,
                        void ***buffers,
                        size_t num_buffers,
                        bladerf_format format,
                        size_t samples_per_buffer,
                        size_t num_transfers,
                        void *data)
{
    printf("bladerf_init_stream .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->init_stream(stream, dev, callback, buffers,
                                     num_buffers, format, samples_per_buffer,
                                     num_transfers, data);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_stream(struct bladerf_stream *stream, bladerf_channel_layout layout)
{
    printf("bladerf_stream .. .. ..\n");
    return stream->dev->board->stream(stream, layout);
}

int bladerf_submit_stream_buffer(struct bladerf_stream *stream,
                                 void *buffer,
                                 unsigned int timeout_ms)
{
    printf("bladerf_submit_stream_buffer .. .. ..\n");
    return stream->dev->board->submit_stream_buffer(stream, buffer, timeout_ms,
                                                    false);
}

int bladerf_submit_stream_buffer_nb(struct bladerf_stream *stream, void *buffer)
{
    printf("bladerf_submit_stream_buffer_nb .. .. ..\n");
    return stream->dev->board->submit_stream_buffer(stream, buffer, 0, true);
}

void bladerf_deinit_stream(struct bladerf_stream *stream)
{
    printf("bladerf_deinit_stream .. .. ..\n");
    if (stream) {
        stream->dev->board->deinit_stream(stream);
    }
}

int bladerf_set_stream_timeout(struct bladerf *dev,
                               bladerf_direction dir,
                               unsigned int timeout)
{
    printf("bladerf_set_stream_timeout .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_stream_timeout(dev, dir, timeout);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_stream_timeout(struct bladerf *dev,
                               bladerf_direction dir,
                               unsigned int *timeout)
{
    printf("bladerf_get_stream_timeout .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_stream_timeout(dev, dir, timeout);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_sync_config(struct bladerf *dev,
                        bladerf_channel_layout layout,
                        bladerf_format format,
                        unsigned int num_buffers,
                        unsigned int buffer_size,
                        unsigned int num_transfers,
                        unsigned int stream_timeout)
{
    printf("bladerf_sync_config .. .. ..\n");

    printf("num_buffers: %u\n", num_buffers);
    printf("buffer_size: %u\n", buffer_size);
    printf("num_transfers: %u\n", num_transfers);
    printf("stream_timeout: %u\n", stream_timeout);

    printf("bladerf_channel_layout: %u\n", layout);
    printf("bladerf_format: %u\n", format);

    int status;
    MUTEX_LOCK(&dev->lock);

    status =
        dev->board->sync_config(dev, layout, format, num_buffers, buffer_size,
                                num_transfers, stream_timeout);
    bladerf_frequency frequency;
    for(unsigned int ch = 0; ch<4; ch=ch+2){
    	status = dev->board->get_frequency(dev, ch, &frequency);
   	 printf("Chan %u Frequency: %lu\n" ,ch, frequency);
	 struct bladerf_rational_rate rate;
	 status = dev->board->get_rational_sample_rate(dev, ch, &rate);
   	 printf("Chan %u rational samplerate: %lu, %lu, %lu\n", ch, rate.integer, rate.num, rate.den);
         bladerf_bandwidth bandwidth;
	 status = dev->board->get_bandwidth(dev, ch, &bandwidth);
         printf("Chan %u bandwidth: %d\n", ch, bandwidth);
         int gain;
         status = dev->board->get_gain(dev, ch, &gain);
         printf("Chan %u Gain: %d\n" , ch, gain);
         bladerf_gain_mode gmode;
         status = dev->board->get_gain_mode(dev, ch, &gmode);
         printf("Chan %u Gain Mode: %d\n", ch, gmode);
    }
    bladerf_rx_mux mux;
    status = dev->board->get_rx_mux(dev, &mux);
    printf("rx mux mode: %d\n", mux);
    unsigned int l;
    status = dev->board->get_loopback(dev, &l);
    printf("loopback mode: %d\n", l);
    bladerf_tuning_mode tmode;
    status = dev->board->get_tuning_mode(dev, &tmode);
    printf("tuning mode: %d\n", tmode);
   /* */

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_sync_tx(struct bladerf *dev,
                    void const *samples,
                    unsigned int num_samples,
                    struct bladerf_metadata *metadata,
                    unsigned int timeout_ms)
{
    printf("bladerf_sync_tx .. .. ..\n");
    return dev->board->sync_tx(dev, samples, num_samples, metadata, timeout_ms);
}

int bladerf_sync_rx(struct bladerf *dev,
                    void *samples,
                    unsigned int num_samples,
                    struct bladerf_metadata *metadata,
                    unsigned int timeout_ms)
{
    static unsigned int instance = 0;

    if((instance % 1000) == 0){
    	printf("bladerf_sync_rx .. .. ..\n");
        printf("num samples: %u\n", num_samples);
	printf("timeout: %u\n", timeout_ms);
        printf("instance: %u\n", instance);
    }
    instance = instance + 1;

    return dev->board->sync_rx(dev, samples, num_samples, metadata, timeout_ms);
}

int bladerf_get_timestamp(struct bladerf *dev,
                          bladerf_direction dir,
                          bladerf_timestamp *timestamp)
{
    printf("bladerf_get_timestamp .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_timestamp(dev, dir, timestamp);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_interleave_stream_buffer(bladerf_channel_layout layout,
                                     bladerf_format format,
                                     unsigned int buffer_size,
                                     void *samples)
{
    printf("bladerf_interleave_stream_buffer .. .. ..\n");
    return _interleave_interleave_buf(layout, format, buffer_size, samples);
}

int bladerf_deinterleave_stream_buffer(bladerf_channel_layout layout,
                                       bladerf_format format,
                                       unsigned int buffer_size,
                                       void *samples)
{
    printf("bladerf_deinterleave_stream_buffer .. .. ..\n");
    return _interleave_deinterleave_buf(layout, format, buffer_size, samples);
}

/******************************************************************************/
/* FPGA/Firmware Loading/Flashing */
/******************************************************************************/

int bladerf_load_fpga(struct bladerf *dev, const char *fpga_file)
{
    printf("bladerf_load_fpga .. .. ..\n");
    uint8_t *buf = NULL;
    size_t buf_size;
    int status;

    status = file_read_buffer(fpga_file, &buf, &buf_size);
    if (status != 0) {
        goto exit;
    }

    status = dev->board->load_fpga(dev, buf, buf_size);

exit:
    free(buf);
    return status;
}

int bladerf_flash_fpga(struct bladerf *dev, const char *fpga_file)
{
    printf("bladerf_flash_fpga .. .. ..\n");
    uint8_t *buf = NULL;
    size_t buf_size;
    int status;

    status = file_read_buffer(fpga_file, &buf, &buf_size);
    if (status != 0) {
        goto exit;
    }

    MUTEX_LOCK(&dev->lock);
    status = dev->board->flash_fpga(dev, buf, buf_size);
    MUTEX_UNLOCK(&dev->lock);

exit:
    free(buf);
    return status;
}

int bladerf_erase_stored_fpga(struct bladerf *dev)
{
    printf("bladerf_erase_stored_fpga .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->erase_stored_fpga(dev);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_flash_firmware(struct bladerf *dev, const char *firmware_file)
{
    printf("bladerf_flash_firmware .. .. ..\n");
    uint8_t *buf = NULL;
    size_t buf_size;
    int status;

    status = file_read_buffer(firmware_file, &buf, &buf_size);
    if (status != 0) {
        goto exit;
    }

    MUTEX_LOCK(&dev->lock);
    status = dev->board->flash_firmware(dev, buf, buf_size);
    MUTEX_UNLOCK(&dev->lock);

exit:
    free(buf);
    return status;
}

int bladerf_device_reset(struct bladerf *dev)
{
    printf("bladerf_device_reset .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->device_reset(dev);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Tuning mode */
/******************************************************************************/

int bladerf_set_tuning_mode(struct bladerf *dev, bladerf_tuning_mode mode)
{
    printf("bladerf_set_tuning_mode .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_tuning_mode(dev, mode);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_tuning_mode(struct bladerf *dev, bladerf_tuning_mode *mode)
{
    //printf("bladerf_get_tuning_mode .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_tuning_mode(dev, mode);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Loopback */
/******************************************************************************/

int bladerf_get_loopback_modes(struct bladerf *dev,
                               struct bladerf_loopback_modes const **modes)
{
    printf("bladerf_get_loopback_modes .. .. ..\n");
    int status;

    status = dev->board->get_loopback_modes(dev, modes);

    return status;
}

bool bladerf_is_loopback_mode_supported(struct bladerf *dev,
                                        bladerf_loopback mode)
{
    printf("bladerf_is_loopback_mode_supported .. .. ..\n");
    struct bladerf_loopback_modes modes;
    struct bladerf_loopback_modes const *modesptr = &modes;
    int i, count;

    count = bladerf_get_loopback_modes(dev, &modesptr);

    for (i = 0; i < count; ++i) {
        if (modesptr[i].mode == mode) {
            return true;
        }
    }

    return false;
}

int bladerf_set_loopback(struct bladerf *dev, bladerf_loopback l)
{
    printf("bladerf_set_loopback .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_loopback(dev, l);
    printf("Mode: %d\n" , l); 
    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_loopback(struct bladerf *dev, bladerf_loopback *l)
{
//    printf("bladerf_get_loopback .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_loopback(dev, l);


    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Sample RX FPGA Mux */
/******************************************************************************/

int bladerf_set_rx_mux(struct bladerf *dev, bladerf_rx_mux mux)
{
    printf("bladerf_set_rx_mux .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_rx_mux(dev, mux);
    printf("Mode: %d\n" , mux); 

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_rx_mux(struct bladerf *dev, bladerf_rx_mux *mux)
{
    //printf("bladerf_get_rx_mux .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_rx_mux(dev, mux);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Low-level VCTCXO Tamer Mode */
/******************************************************************************/

int bladerf_set_vctcxo_tamer_mode(struct bladerf *dev,
                                  bladerf_vctcxo_tamer_mode mode)
{
    printf("bladerf_set_vctcxo_tamer_mode .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->set_vctcxo_tamer_mode(dev, mode);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_get_vctcxo_tamer_mode(struct bladerf *dev,
                                  bladerf_vctcxo_tamer_mode *mode)
{
    printf("bladerf_get_vctcxo_tamer_mode .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_vctcxo_tamer_mode(dev, mode);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Low-level VCTCXO Trim DAC access */
/******************************************************************************/

int bladerf_get_vctcxo_trim(struct bladerf *dev, uint16_t *trim)
{
    printf("bladerf_get_vctcxo_trim .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->get_vctcxo_trim(dev, trim);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_trim_dac_read(struct bladerf *dev, uint16_t *trim)
{
    printf("bladerf_trim_dac_read .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->trim_dac_read(dev, trim);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_trim_dac_write(struct bladerf *dev, uint16_t trim)
{
    printf("bladerf_trim_dac_write .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->trim_dac_write(dev, trim);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_dac_read(struct bladerf *dev, uint16_t *trim)
{
    printf("bladerf_dac_read .. .. ..\n");
    return bladerf_trim_dac_read(dev, trim);
}
int bladerf_dac_write(struct bladerf *dev, uint16_t trim)
{
    printf("bladerf_dac_write .. .. ..\n");
    return bladerf_trim_dac_write(dev, trim);
}

/******************************************************************************/
/* Low-level Trigger control access */
/******************************************************************************/

int bladerf_read_trigger(struct bladerf *dev,
                         bladerf_channel ch,
                         bladerf_trigger_signal trigger,
                         uint8_t *val)
{
    printf("bladerf_read_trigger .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->read_trigger(dev, ch, trigger, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_write_trigger(struct bladerf *dev,
                          bladerf_channel ch,
                          bladerf_trigger_signal trigger,
                          uint8_t val)
{
    printf("bladerf_write_trigger .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->write_trigger(dev, ch, trigger, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Low-level Configuration GPIO access */
/******************************************************************************/

int bladerf_config_gpio_read(struct bladerf *dev, uint32_t *val)
{
    printf("bladerf_config_gpio_read .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->config_gpio_read(dev, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_config_gpio_write(struct bladerf *dev, uint32_t val)
{
    printf("bladerf_config_gpio_write .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->config_gpio_write(dev, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Low-level SPI Flash access */
/******************************************************************************/

int bladerf_erase_flash(struct bladerf *dev,
                        uint32_t erase_block,
                        uint32_t count)
{
    printf("bladerf_erase_flash .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->erase_flash(dev, erase_block, count);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_erase_flash_bytes(struct bladerf *dev,
                              uint32_t address,
                              uint32_t length)
{
    printf("bladerf_erase_flash_bytes .. .. ..\n");
    int      status;
    uint32_t eb;
    uint32_t count;

    /* Make sure address is aligned to an erase block boundary */
    if( (address % dev->flash_arch->ebsize_bytes) == 0 ) {
        /* Convert into units of flash pages */
        eb = address / dev->flash_arch->ebsize_bytes;
    } else {
        log_error("Address or length not aligned on a flash page boundary.\n");
        return BLADERF_ERR_INVAL;
    }

    /* Check for the case of erasing less than 1 erase block.
     * For example, the calibration data. If so, round up to 1 EB.
     * If erasing more than 1 EB worth of data, make sure the length
     * is aligned to an EB boundary. */
    if( (length > 0) && (length < dev->flash_arch->ebsize_bytes) ) {
        count = 1;
    } else if ((length % dev->flash_arch->ebsize_bytes) == 0) {
        /* Convert into units of flash pages */
        count = length  / dev->flash_arch->ebsize_bytes;
    } else {
        log_error("Address or length not aligned on a flash page boundary.\n");
        return BLADERF_ERR_INVAL;
    }

    status = bladerf_erase_flash(dev, eb, count);

    return status;
}

int bladerf_read_flash(struct bladerf *dev,
                       uint8_t *buf,
                       uint32_t page,
                       uint32_t count)
{
    printf("bladerf_read_flash .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->read_flash(dev, buf, page, count);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_read_flash_bytes(struct bladerf *dev,
                             uint8_t *buf,
                             uint32_t address,
                             uint32_t length)
{
    printf("bladerf_read_flash_bytes .. .. ..\n");
    int      status;
    uint32_t page;
    uint32_t count;

    /* Check alignment */
    if( ((address % dev->flash_arch->psize_bytes) != 0) ||
        ((length  % dev->flash_arch->psize_bytes) != 0) ) {
        log_error("Address or length not aligned on a flash page boundary.\n");
        return BLADERF_ERR_INVAL;
    }

    /* Convert into units of flash pages */
    page  = address / dev->flash_arch->psize_bytes;
    count = length  / dev->flash_arch->psize_bytes;

    status = bladerf_read_flash(dev, buf, page, count);

    return status;
}

int bladerf_write_flash(struct bladerf *dev,
                        const uint8_t *buf,
                        uint32_t page,
                        uint32_t count)
{
    printf("bladerf_write_flash .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->write_flash(dev, buf, page, count);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_write_flash_bytes(struct bladerf *dev,
                              const uint8_t *buf,
                              uint32_t address,
                              uint32_t length)
{
    printf("bladerf_write_flash_bytes .. .. ..\n");
    int      status;
    uint32_t page;
    uint32_t count;

    /* Check alignment */
    if( ((address % dev->flash_arch->psize_bytes) != 0) ||
        ((length  % dev->flash_arch->psize_bytes) != 0) ) {
        log_error("Address or length not aligned on a flash page boundary.\n");
        return BLADERF_ERR_INVAL;
    }

    /* Convert address and length into units of flash pages */
    page  = address / dev->flash_arch->psize_bytes;
    count = length  / dev->flash_arch->psize_bytes;

    status = bladerf_write_flash(dev, buf, page, count);
    return status;
}

int bladerf_read_otp(struct bladerf *dev,
                       uint8_t *buf)
{
    printf("bladerf_read_otp .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->backend->get_otp(dev, (char *)buf);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_write_otp(struct bladerf *dev,
                        uint8_t *buf)
{
    printf("bladerf_write_otp .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->backend->write_otp(dev, (char *)buf);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_lock_otp(struct bladerf *dev)
{
    printf("bladerf_lock_otp .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->backend->lock_otp(dev);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/******************************************************************************/
/* Helpers & Miscellaneous */
/******************************************************************************/

const char *bladerf_strerror(int error)
{
    printf("bladerf_strerror .. .. ..\n");
    switch (error) {
        case BLADERF_ERR_UNEXPECTED:
            return "An unexpected error occurred";
        case BLADERF_ERR_RANGE:
            return "Provided parameter was out of the allowable range";
        case BLADERF_ERR_INVAL:
            return "Invalid operation or parameter";
        case BLADERF_ERR_MEM:
            return "A memory allocation error occurred";
        case BLADERF_ERR_IO:
            return "File or device I/O failure";
        case BLADERF_ERR_TIMEOUT:
            return "Operation timed out";
        case BLADERF_ERR_NODEV:
            return "No devices available";
        case BLADERF_ERR_UNSUPPORTED:
            return "Operation not supported";
        case BLADERF_ERR_MISALIGNED:
            return "Misaligned flash access";
        case BLADERF_ERR_CHECKSUM:
            return "Invalid checksum";
        case BLADERF_ERR_NO_FILE:
            return "File not found";
        case BLADERF_ERR_UPDATE_FPGA:
            return "An FPGA update is required";
        case BLADERF_ERR_UPDATE_FW:
            return "A firmware update is required";
        case BLADERF_ERR_TIME_PAST:
            return "Requested timestamp is in the past";
        case BLADERF_ERR_QUEUE_FULL:
            return "Could not enqueue data into full queue";
        case BLADERF_ERR_FPGA_OP:
            return "An FPGA operation reported a failure";
        case BLADERF_ERR_PERMISSION:
            return "Insufficient permissions for the requested operation";
        case BLADERF_ERR_WOULD_BLOCK:
            return "The operation would block, but has been requested to be "
                   "non-blocking";
        case BLADERF_ERR_NOT_INIT:
            return "Insufficient initialization for the requested operation";
        case 0:
            return "Success";
        default:
            return "Unknown error code";
    }
}

const char *bladerf_backend_str(bladerf_backend backend)
{
    printf("bladerf_backend_str .. .. ..\n");
    return backend2str(backend);
}

void bladerf_log_set_verbosity(bladerf_log_level level)
{
    printf("bladerf_log_set_verbosity .. .. ..\n");
    log_set_verbosity(level);
#if defined(LOG_SYSLOG_ENABLED)
    log_debug("Log verbosity has been set to: %d", level);
#endif
}

void bladerf_set_usb_reset_on_open(bool enabled)
{
    printf("bladerf_set_usb_reset_on_open .. .. ..\n");
#if ENABLE_USB_DEV_RESET_ON_OPEN
    bladerf_usb_reset_device_on_open = enabled;

    log_verbose("USB reset on open %s\n", enabled ? "enabled" : "disabled");
#else
    log_verbose("%s has no effect. "
                "ENABLE_USB_DEV_RESET_ON_OPEN not set at compile-time.\n",
                __FUNCTION__);
#endif
}

/******************************************************************************/
/* Expansion board APIs */
/******************************************************************************/

int bladerf_expansion_attach(struct bladerf *dev, bladerf_xb xb)
{
    printf("bladerf_expansion_attach .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->expansion_attach(dev, xb);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_expansion_get_attached(struct bladerf *dev, bladerf_xb *xb)
{
    printf("bladerf_expansion_get_attached .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = dev->board->expansion_get_attached(dev, xb);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/* XB100 */

int bladerf_expansion_gpio_read(struct bladerf *dev, uint32_t *val)
{
    printf("bladerf_expansion_gpio_read .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb100_gpio_read(dev, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_expansion_gpio_write(struct bladerf *dev, uint32_t val)
{
    printf("bladerf_expansion_gpio_write .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb100_gpio_write(dev, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_expansion_gpio_masked_write(struct bladerf *dev,
                                        uint32_t mask,
                                        uint32_t val)
{
    printf("bladerf_expansion_gpio_masked_write .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb100_gpio_masked_write(dev, mask, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_expansion_gpio_dir_read(struct bladerf *dev, uint32_t *val)
{
    printf("bladerf_expansion_gpio_dir_read .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb100_gpio_dir_read(dev, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_expansion_gpio_dir_write(struct bladerf *dev, uint32_t val)
{
    printf("bladerf_expansion_gpio_dir_write .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb100_gpio_dir_write(dev, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_expansion_gpio_dir_masked_write(struct bladerf *dev,
                                            uint32_t mask,
                                            uint32_t val)
{
    printf("bladerf_expansion_gpio_dir_masked_write .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb100_gpio_dir_masked_write(dev, mask, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/* XB200 */

int bladerf_xb200_set_filterbank(struct bladerf *dev,
                                 bladerf_channel ch,
                                 bladerf_xb200_filter filter)
{
    printf("bladerf_xb200_set_filterbank .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb200_set_filterbank(dev, ch, filter);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_xb200_get_filterbank(struct bladerf *dev,
                                 bladerf_channel ch,
                                 bladerf_xb200_filter *filter)
{
    printf("bladerf_xb200_get_filterbank .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb200_get_filterbank(dev, ch, filter);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_xb200_set_path(struct bladerf *dev,
                           bladerf_channel ch,
                           bladerf_xb200_path path)
{
    printf("bladerf_xb200_set_path .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb200_set_path(dev, ch, path);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_xb200_get_path(struct bladerf *dev,
                           bladerf_channel ch,
                           bladerf_xb200_path *path)
{
    printf("bladerf_xb200_get_path .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb200_get_path(dev, ch, path);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

/* XB300 */

int bladerf_xb300_set_trx(struct bladerf *dev, bladerf_xb300_trx trx)
{
    printf("bladerf_xb300_set_trx .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb300_set_trx(dev, trx);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_xb300_get_trx(struct bladerf *dev, bladerf_xb300_trx *trx)
{
    printf("bladerf_xb300_get_trx .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb300_get_trx(dev, trx);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_xb300_set_amplifier_enable(struct bladerf *dev,
                                       bladerf_xb300_amplifier amp,
                                       bool enable)
{
    printf("bladerf_xb300_set_amplifier_enable .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb300_set_amplifier_enable(dev, amp, enable);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_xb300_get_amplifier_enable(struct bladerf *dev,
                                       bladerf_xb300_amplifier amp,
                                       bool *enable)
{
    printf("bladerf_xb300_get_amplifier_enable .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb300_get_amplifier_enable(dev, amp, enable);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}

int bladerf_xb300_get_output_power(struct bladerf *dev, float *val)
{
    printf("bladerf_xb300_get_output_power .. .. ..\n");
    int status;
    MUTEX_LOCK(&dev->lock);

    status = xb300_get_output_power(dev, val);

    MUTEX_UNLOCK(&dev->lock);
    return status;
}
