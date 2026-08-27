/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <service/controller.hpp>

namespace sight::module::data
{

/**
 * @brief This service resets an object using the default constructor.
 * It resets one or several objects using the "data.target" group.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::data::reset" >
       <data target="..." />
       <data target="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b data.target [sight::data::object]: define the target objects to reset.
 */
class reset final : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(reset, service::controller);

    /// Creates the service.
    reset() = default;

    /// Destroys the service.
    ~reset() final = default;

protected:

    /// Does nothing.
    void starting() final;

    /// Resets the object(s)
    void updating() final;

    /// Does nothing.
    void stopping() final;

private:

    /// Resets the target objects.
    void reset_targets();

    sight::data::ptr_vector<sight::data::object, sight::data::access::inout> m_targets {this, "data.target"};
};

} // namespace sight::module::data.
