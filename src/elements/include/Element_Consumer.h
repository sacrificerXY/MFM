/*                                              -*- mode:C++ -*-
  Element_Consumer.h Basic data consuming element
  Copyright (C) 2014 The Regents of the University of New Mexico.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
  USA
*/

/**
  \file Element_Consumer.h Basic data consuming element
  \author Trent R. Small.
  \author David H. Ackley.
  \date (C) 2014 All rights reserved.
  \lgpl
 */
#ifndef ELEMENT_CONSUMER_H
#define ELEMENT_CONSUMER_H

#include "Element.h"
#include "EventWindow.h"
#include "ElementTable.h"
#include "Element_Data.h"
#include "Element_Empty.h"
#include "Element_Emitter.h" /* For DATA_MAXVAL, DATA_MINVAL */
#include "AbstractElement_Reprovert.h"
#include "itype.h"
#include "Util.h"
#include "Tile.h"

namespace MFM
{
  template <class EC>
  class Element_Consumer : public AbstractElement_Reprovert<EC>
  {
  public:
    virtual u32 GetTypeFromThisElement() const {
      return 0xCE06;
    }
    
    // Extract short names for parameter types
    typedef typename EC::ATOM_CONFIG AC;
    typedef typename AC::ATOM_TYPE T;
    enum { R = EC::EVENT_WINDOW_RADIUS };

  public:
    // Element Data Slot names
    enum {
      DATUMS_CONSUMED_SLOT,
      TOTAL_BUCKET_ERROR_SLOT,
      DATA_SLOT_COUNT
    };

    static Element_Consumer THE_INSTANCE;

    virtual T BuildDefaultAtom() const {
      T defaultAtom(this->GetType(), 0, 0, AbstractElement_Reprovert<EC>::STATE_BITS);
      this->SetGap(defaultAtom,1); // Pack consumers adjacent
      return defaultAtom;
    }

    Element_Consumer()
      : AbstractElement_Reprovert<EC>(MFM_UUID_FOR("Consumer",1))
    {
      Element<EC>::SetAtomicSymbol("Cn");
      Element<EC>::SetName("Consumer");
    }

    u64 GetAndResetDatumsConsumed(Tile<EC> & t) const
    {
      ElementTable<EC> & et = t.GetElementTable();
      u64 * datap = et.GetDataIfRegistered(this->GetType(), DATA_SLOT_COUNT);
      if (!datap)
        return 0;

      u64 ret = datap[DATUMS_CONSUMED_SLOT];
      datap[DATUMS_CONSUMED_SLOT] = 0;
      return ret;
    }

    u64 GetAndResetBucketError(Tile<EC> & t) const
    {
      ElementTable<EC> & et = t.GetElementTable();
      u64 * datap = et.GetDataIfRegistered(this->GetType(), DATA_SLOT_COUNT);
      if (!datap)
        return 0;

      u64 ret = datap[TOTAL_BUCKET_ERROR_SLOT];
      datap[TOTAL_BUCKET_ERROR_SLOT] = 0;
      return ret;
    }

    virtual u32 PercentMovable(const T& you,
                               const T& me, const SPoint& offset) const
    {
      return 0;
    }

    virtual u32 GetElementColor() const
    {
      return 0xff202030;
    }

    virtual const char* GetDescription() const
    {
      return "This vertically-reproducing Element consumes DATA atoms and holds "
             "information on its position and the DATA consumed.";
    }

    virtual void Behavior(EventWindow<EC>& window) const
    {
      //      Random & random = window.GetRandom();
      this->ReproduceVertically(window);

      T self = window.GetCenterAtomSym();

      // Find nearest-on-right consumable, if any
      for (u32 i = 1; i < R; ++i)
      {
        SPoint consPt(i,0);
        if(window.GetRelativeAtomSym(consPt).GetType() == Element_Data<EC>::TYPE())
        {
            // Use the expanded info maintained by reprovert to
            // dynamically compute the bucket size.  That info can be
            // temporarily disrupted e.g. by DReg, so going this route
            // does increase the overall sorting error a bit.  But it's so
            // damn robust.  Turn off an output tile and it starts sorting

            u32 above = this->GetAbove(self,0);
            u32 below = this->GetBelow(self,0);
            u32 range = above + below + 1;
            const u32 bucketSize = DATA_MAXVAL / range;

            u32 val = Element_Data<EC>::THE_INSTANCE.GetDatum(window.GetRelativeAtomSym(consPt), 0);

            u32 minBucketVal = bucketSize * above;
            u32 maxBucketVal = bucketSize * (above + 1);
            u32 midpoint = (maxBucketVal+minBucketVal)/2;

            u32 diff;
            if (val < midpoint) diff = midpoint - val;
            else diff = val - midpoint;

            u32 bucketsOff = diff / bucketSize;

#if 0 //XXX  Fri Jan 30 22:13:11 2015 EDS unreimplemented

            Tile<EC> & tile = window.GetTile();
            ElementTable<EC> & et = tile.GetElementTable();

            u64 * datap = et.GetDataAndRegister(this->GetType(), DATA_SLOT_COUNT);
            ++datap[DATUMS_CONSUMED_SLOT];                 // Count datums consumed
            datap[TOTAL_BUCKET_ERROR_SLOT] += bucketsOff;  // Count total bucket error
#endif
            LOG.Debug("Consumed %d bucketsOff",bucketsOff);

            /*
              printf("[%d:%d:%d/bs %d>%d<%d=%d]Export!: %d %ld %ld %f\n",
              below,
              range,
              above,
              bucketSize,
              minBucketVal,
              maxBucketVal,
              bucketsOff,
              val,
              datap[DATUMS_CONSUMED_SLOT], datap[TOTAL_BUCKET_ERROR_SLOT],
              ((double)datap[TOTAL_BUCKET_ERROR_SLOT])/datap[DATUMS_CONSUMED_SLOT]);
            */
            window.SetRelativeAtomSym(consPt, Element_Empty<EC>::THE_INSTANCE.GetDefaultAtom());
            break;
          }
      }
    }
  };

  template <class EC>
  Element_Consumer<EC> Element_Consumer<EC>::THE_INSTANCE;
}

#endif /* ELEMENT_CONSUMER_H */
