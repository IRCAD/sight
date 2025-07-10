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

#include "core/jobs/has_jobs.hpp"

#include <core/com/signal.hxx>

namespace sight::core::jobs
{

//------------------------------------------------------------------------------

has_jobs::has_jobs(sight::core::com::signals& _signals) noexcept
{
    _signals(signals::JOB_CREATED, std::make_shared<signals::job_created_t>());
}

//------------------------------------------------------------------------------

} // namespace sight::core::jobs
