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


#ifndef INCLUDED_BLADERF_SINGLE_RX_H
#define INCLUDED_BLADERF_SINGLE_RX_H

#include <bladerf/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace bladerf {

    /*!
     * \brief <+description of block+>
     * \ingroup bladerf
     *
     */
    class BLADERF_API single_rx : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<single_rx> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of bladerf::single_rx.
       *
       * To avoid accidental use of raw pointers, bladerf::single_rx's
       * constructor is in a private implementation
       * class. bladerf::single_rx::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace bladerf
} // namespace gr

#endif /* INCLUDED_BLADERF_SINGLE_RX_H */

