/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "core/com/connection.hpp"
#include "core/config.hpp"

#include <core/base_object.hpp>

namespace sight::core::com
{

struct slot_base;

/**
 * @brief Signal base class.
 */
struct CORE_CLASS_API signal_base : virtual core::base_object
{
    /**
     * @name Typedefs
     * @{ */
    typedef SPTR(signal_base) sptr;
    typedef WPTR(signal_base)   wptr;
    /**  @} */

    signal_base()
    = default;

    ~signal_base() override
    = default;

    /**
     * @brief Connects the given slot.
     * @throws BadSlot If given slot doesn't match signal type.
     */
    virtual connection connect(SPTR(slot_base) slot) = 0;

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    virtual void disconnect(SPTR(slot_base) slot) = 0;

    /// Returns number of connections.
    virtual std::size_t num_connections() const = 0;

    protected:

        /// Copy constructor forbidden
        signal_base(const signal_base&);

        /// Copy operator forbiden
        signal_base& operator=(const signal_base&);
};

} // namespace sight::core::com
