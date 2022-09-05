/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "core/config.hpp"

#include <core/FactoryRegistry.hpp>
#include <core/macros.hpp>

#include <string>

namespace sight::core::memory
{

class IPolicy;

namespace policy::registry
{

typedef std::string KeyType;

typedef core::FactoryRegistry<SPTR(core::memory::IPolicy)(), KeyType> Type;

CORE_API SPTR(Type) get();

} // namespace policy::registry

} // namespace sight::core::memory
