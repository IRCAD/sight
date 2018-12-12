/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/
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
    typedef std::shared_ptr< SlotConnectionBase > sptr;
    typedef std::shared_ptr< SlotConnectionBase const > csptr;
    typedef std::weak_ptr< SlotConnectionBase > wptr;
    typedef std::weak_ptr< SlotConnectionBase const > cwptr;

    typedef std::shared_ptr< void > BlockerSptrType;
    typedef std::weak_ptr< void >   BlockerWptrType;
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

