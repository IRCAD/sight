/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <service/base.hpp>

namespace ex_progress_bar
{

class long_job : public sight::service::base,
                 public sight::core::notification::has_monitors
{
public:

    SIGHT_DECLARE_SERVICE(long_job, sight::service::base);

    /// Constructor.
    long_job() noexcept;

    /// Destructor.
    ~long_job() noexcept override = default;

protected:

    /// Initialize the container.
    void configuring() override;

    /// Start the service, slot connections
    void starting() override;

    /// Destroy the service
    void stopping() override;

    /// Update the interface.
    void updating() override;

private:

    bool m_cancelable {true};
};

} // namespace ex_progress_bar.
