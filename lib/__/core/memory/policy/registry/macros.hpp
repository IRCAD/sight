/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/memory/policy/base.hpp"
#include "core/memory/policy/registry/detail.hpp"

#include <boost/preprocessor/cat.hpp>

namespace sight::core::memory::policy::registry
{

#define SIGHT_REGISTER_MEMORY_POLICY(classname) \
        static core::memory::policy::base::registry<classname> s__factory__record__ ## __LINE__;

} // namespace sight::core::memory::policy::registry
