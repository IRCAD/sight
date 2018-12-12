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
#ifndef __FWCOM_SIGNALBASE_HPP__
#define __FWCOM_SIGNALBASE_HPP__

#include <fwCore/BaseObject.hpp>

#include "fwCom/config.hpp"

#include "fwCom/Connection.hpp"

namespace fwCom
{

struct SlotBase;

/**
 * @brief Signal base class.
 */
struct SignalBase : virtual fwCore::BaseObject
{
    /**
     * @name Typedefs
     * @{ */
    typedef SPTR ( SignalBase ) sptr;
    typedef WPTR ( SignalBase )   wptr;
    /**  @} */

    SignalBase()
    {
    }

    virtual ~SignalBase()
    {
    }

    /**
     * @brief Connects the given slot.
     * @throws BadSlot If given slot doesn't match signal type.
     */
    virtual Connection connect ( SPTR( SlotBase ) slot ) = 0;

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    virtual void disconnect ( SPTR( SlotBase ) slot ) = 0;

    /// Returns number of connections.
    virtual size_t getNumberOfConnections() const = 0;

    protected:
        /// Copy constructor forbidden
        SignalBase( const SignalBase& );

        /// Copy operator forbiden
        SignalBase& operator=( const SignalBase& );
};

} // namespace fwCom

#endif /* __FWCOM_SIGNALBASE_HPP__ */

