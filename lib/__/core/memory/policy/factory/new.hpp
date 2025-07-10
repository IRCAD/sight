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

#include <sight/core/config.hpp>

#include "core/memory/policy/registry/detail.hpp"

#include <string>

namespace sight::core::memory::policy::factory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  create();

SIGHT_CORE_API SPTR(core::memory::policy::base) create(const core::memory::policy::registry::key_t& _classname);

template<class CLASSNAME>
SPTR(CLASSNAME)  create()
{
    SPTR(CLASSNAME) policy = std::make_shared<CLASSNAME>();
    return policy;
}

} // namespace sight::core::memory::policy::factory
