/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTCONNECTIONBASE_HPP__
#define __FWCOM_SLOTCONNECTIONBASE_HPP__

#include <fwCore/BaseObject.hpp>

#include "fwCom/config.hpp"

namespace fwCom
{

/**
 * @brief Base class for Slot connection implementation.
 * This class is for internal use purpose.
 */
struct SlotConnectionBase : virtual fwCore::BaseObject
{
    /**
     * @name Typedefs
     * @{ */
    typedef ::boost::shared_ptr< SlotConnectionBase > sptr;
    typedef ::boost::shared_ptr< SlotConnectionBase const > csptr;
    typedef ::boost::weak_ptr< SlotConnectionBase > wptr;
    typedef ::boost::weak_ptr< SlotConnectionBase const > cwptr;

    typedef boost::shared_ptr< void > BlockerSptrType;
    typedef boost::weak_ptr< void >   BlockerWptrType;
    /**  @} */

    SlotConnectionBase()
    {
    }

    /// Disconnect a Slot from this connection.
    virtual void disconnect() = 0;

    /// Returns a .. to block this connection.
    virtual BlockerSptrType getBlocker() = 0;

    protected:

        /// Copy constructor forbidden
        SlotConnectionBase( const SlotConnectionBase& );

        /// Copy operator forbidden
        SlotConnectionBase& operator=( const SlotConnectionBase& );

        /**
         * @name SlotConnectionBase's friends
         * @{ */
        template < typename F >
        friend struct Signal;
        /**  @} */

        /// *NOT THREAD SAFE* Disconnect a Slot from this connection.
        virtual void disconnectWeakLock() = 0;


        /// *NOT THREAD SAFE* Connect the related Signal and Slot together.
        virtual void connectNoLock() = 0;
};

} // namespace fwCom

#endif /* __FWCOM_SLOTCONNECTIONBASE_HPP__ */

