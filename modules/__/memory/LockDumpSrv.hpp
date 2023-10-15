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

#include "modules/memory/config.hpp"

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
   <service type="sight::module::memory::LockDumpSrv">
       <inout key="target" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b target [sight::data::object]: object to dump lock.
 */
class MODULE_MEMORY_CLASS_API LockDumpSrv : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(LockDumpSrv, sight::service::controller);

    /// Constructor. Does nothing
    MODULE_MEMORY_API LockDumpSrv() noexcept;

    /// Destructor. Does nothing
    MODULE_MEMORY_API ~LockDumpSrv() noexcept override;

protected:

    /// Uses data::ObjectLock to dump lock the associated data
    MODULE_MEMORY_API void starting() override;

    /// Dump unlock the associated data
    MODULE_MEMORY_API void stopping() override;

    /// Does nothing
    MODULE_MEMORY_API void updating() override;

    /// Does nothing
    MODULE_MEMORY_API void info(std::ostream& _sstream) override;

    /// Does nothing
    MODULE_MEMORY_API void configuring() override;

private:

    sight::data::ptr<sight::data::object, sight::data::Access::inout> m_target {this, "target"};

    std::vector<core::memory::buffer_object::lock_t> m_locks;
};

} // namespace sight::module::memory
