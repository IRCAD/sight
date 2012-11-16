/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTCONNECTIONBASE_HPP__
#define __FWCOM_SLOTCONNECTIONBASE_HPP__

#include <fwCore/BaseObject.hpp>

#include "fwCom/config.hpp"

namespace fwCom
{

struct SlotConnectionBase: virtual fwCore::BaseObject
{
    typedef ::boost::shared_ptr< SlotConnectionBase > sptr;
    typedef ::boost::shared_ptr< SlotConnectionBase const > csptr;
    typedef ::boost::weak_ptr< SlotConnectionBase > wptr;
    typedef ::boost::weak_ptr< SlotConnectionBase const > cwptr;

    typedef boost::shared_ptr< void > BlockerSptrType;
    typedef boost::weak_ptr< void >   BlockerWptrType;


    virtual void disconnect() = 0;

    virtual BlockerSptrType getBlocker() = 0;

protected:
    virtual void connect() = 0;
};

} // namespace fwCom

#endif /* __FWCOM_SLOTCONNECTIONBASE_HPP__ */

