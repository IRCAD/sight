/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/memory/policy/factory/new.hpp"

#include "core/memory/policy/base.hpp"
#include "core/memory/policy/registry/detail.hpp"

namespace sight::core::memory::policy::factory
{

//------------------------------------------------------------------------------

core::memory::policy::base::sptr create(const core::memory::policy::registry::key_type& classname)
{
    return core::memory::policy::registry::get()->create(classname);
}

} // namespace sight::core::memory::policy::factory
