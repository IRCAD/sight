/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

// This file is only kept for compatibility issue with Sight < 20.0

#pragma once

#include <fwRuntime/Profile.hpp>

namespace fwRuntime
{
// To keep access notably to getCurrentProfile()
namespace profile = ::fwRuntime;

namespace io
{
namespace ProfileReader
{

FWRUNTIME_API std::shared_ptr< ::fwRuntime::Profile > createProfile(const std::filesystem::path& path );
}
}

}
