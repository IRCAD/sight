/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <core/memory/buffer_object.hpp>

#include <service/controller.hpp>

namespace sight::module::memory
{

/**
 * @brief This service performs dumps-locks on objects. They are unlocked when the service stops.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::memory::lock_dump">
       <inout key="target" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b target [sight::data::object]: object to dump lock.
 */
class lock_dump : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(lock_dump, sight::service::controller);

    /// Constructor. Does nothing
    lock_dump() noexcept;

    /// Destructor. Does nothing
    ~lock_dump() noexcept override;

protected:

    /// Uses data::ObjectLock to dump lock the associated data
    void starting() override;

    /// Dump unlock the associated data
    void stopping() override;

    /// Does nothing
    void updating() override;

    /// Does nothing
    void info(std::ostream& _sstream) override;

    /// Does nothing
    void configuring() override;

private:

    sight::data::ptr<sight::data::object, sight::data::access::inout> m_target {this, "target"};

    std::vector<core::memory::buffer_object::lock_t> m_locks;
};

} // namespace sight::module::memory
