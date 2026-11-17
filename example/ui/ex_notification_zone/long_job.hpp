/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <core/notification/has_monitors.hpp>
#include <service/controller.hpp>

namespace ex_notification_zone
{

class long_job final : public sight::service::controller,
                       public sight::core::notification::has_monitors
{
public:

    SIGHT_DECLARE_SERVICE(long_job, sight::service::controller);

    /// Constructor.
    long_job() noexcept;

    /// Destructor.
    ~long_job() noexcept final = default;

protected:

    /// Initialize the container.
    void configuring() final;

    /// Start the service, slot connections
    void starting() final;

    /// Destroy the service
    void stopping() final;

    /// Update the interface.
    void updating() final;

private:

    /// Properties
    sight::data::property<sight::data::boolean> m_cancelable {this, "cancelable", true};
};

} // namespace ex_notification_zone.
