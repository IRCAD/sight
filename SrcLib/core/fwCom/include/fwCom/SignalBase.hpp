/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SIGNALBASE_HPP__
#define __FWCOM_SIGNALBASE_HPP__

#include <fwCore/BaseObject.hpp>

#include "fwCom/config.hpp"

#include "fwCom/Connection.hpp"
#include "fwCom/SlotBase.hpp"

namespace fwCom
{

struct SignalBase : virtual fwCore::BaseObject
{
    typedef SPTR( SignalBase ) sptr;
    typedef WPTR( SignalBase )   wptr;

    virtual ~SignalBase(){};
    virtual Connection connect ( SlotBase::sptr slot ) = 0;
    virtual void disconnect ( SlotBase::sptr slot ) = 0;
    virtual size_t getNumberOfConnections() const = 0;
};

} // namespace fwCom

#endif /* __FWCOM_SIGNALBASE_HPP__ */

