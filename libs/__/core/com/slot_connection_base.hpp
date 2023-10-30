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

#include "core/config.hpp"

#include <core/base_object.hpp>

namespace sight::core::com
{

/**
 * @brief Base class for Slot connection implementation.
 * This class is for internal use purpose.
 */
struct CORE_CLASS_API slot_connection_base : core::base_object
{
    /**
     * @name Typedefs
     * @{ */
    using sptr  = std::shared_ptr<slot_connection_base>;
    using csptr = std::shared_ptr<const slot_connection_base>;
    using wptr  = std::weak_ptr<slot_connection_base>;
    using cwptr = std::weak_ptr<const slot_connection_base>;

    using blocker_sptr_type = std::shared_ptr<void>;
    using blocker_wptr_type = std::weak_ptr<void>;
    /**  @} */

    slot_connection_base()
    = default;

    /// Disconnect a Slot from this connection.
    virtual void disconnect() = 0;

    /// Returns a .. to block this connection.
    virtual blocker_sptr_type get_blocker() = 0;

    protected:

        /// Copy constructor forbidden
        slot_connection_base(const slot_connection_base&);

        /// Copy operator forbidden
        slot_connection_base& operator=(const slot_connection_base&);

        /**
         * @name SlotConnectionBase's friends
         * @{ */
        template<typename F>
        friend struct signal;
        /**  @} */

        /// *NOT THREAD SAFE* Disconnect a Slot from this connection.
        virtual void disconnect_weak_lock() = 0;

        /// *NOT THREAD SAFE* Connect the related Signal and Slot together.
        virtual void connect_no_lock() = 0;
};

} // namespace sight::core::com
