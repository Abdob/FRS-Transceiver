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

#ifndef INCLUDED_BLADERF_SINGLE_RX_IMPL_H
#define INCLUDED_BLADERF_SINGLE_RX_IMPL_H

#include <bladerf/single_rx.h>
#include <iostream>
#include <libbladeRF.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Save to a file, e.g. boilerplate.c, and then compile:
 * $ gcc boilerplate.c -o libbladeRF_example_boilerplate -lbladeRF
 */



namespace gr {
  namespace bladerf {

    class single_rx_impl : public single_rx
    {
     private:
      struct bladerf *_dev;
      int16_t *rx_samples;


     public:
      single_rx_impl();
      ~single_rx_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace bladerf
} // namespace gr

#endif /* INCLUDED_BLADERF_SINGLE_RX_IMPL_H */

