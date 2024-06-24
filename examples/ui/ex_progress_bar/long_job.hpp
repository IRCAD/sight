/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <sight/ex_progress_bar/config.hpp>

#include <core/jobs/base.hpp>

#include <service/base.hpp>

namespace ex_progress_bar
{

class long_job : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(long_job, sight::service::base);

    /// Constructor.
    long_job() noexcept;

    /// Destructor.
    ~long_job() noexcept override = default;

    struct signals final
    {
        using job_created_t = sight::core::com::signal<void (sight::core::jobs::base::sptr)>;
        using key_t         = sight::core::com::signals::key_t;

        static inline const key_t JOB_CREATED = "job_created";
    };

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
