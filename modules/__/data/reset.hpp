/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::data::reset" >
    <inout key="target" uid="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b target [sight::data::object]: define the target object to reset.
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

    //// Configures the service
    void configuring(const config_t& _config) final;

    /// Does nothing.
    void starting() final;

    /// Resets the object
    void updating() final;

    /// Sets the output to null.
    void stopping() final;

private:

    sight::data::ptr<sight::data::object, sight::data::access::inout> m_target {this, "target", true};
};

} // namespace sight::module::data.
