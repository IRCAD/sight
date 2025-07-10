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

#include <sight/core/config.hpp>

#include <core/com/signals.hpp>
#include <core/jobs/job.hpp>

namespace sight::core::jobs
{

/**
 * @brief   Interface for objects that offer progress feedback with jobs.
 */
class SIGHT_CORE_CLASS_API has_jobs
{
public:

    struct signals final
    {
        using job_created_t = sight::core::com::signal<void (sight::core::jobs::base::sptr)>;
        static inline const sight::core::com::signals::key_t JOB_CREATED = "job_created";
    };

protected:

    /// Constructor. Create all default slots.
    /// @param _signals map of signals of the child class. For services, pass the protected variable base::m_signals.
    SIGHT_CORE_API has_jobs(sight::core::com::signals& _signals) noexcept;

    /// Destructor
    SIGHT_CORE_API virtual ~has_jobs() noexcept = default;
};

//------------------------------------------------------------------------------

} // namespace sight::core::jobs
