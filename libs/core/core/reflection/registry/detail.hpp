/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <core/macros.hpp>
#include <core/tools/Object.hpp>
#include <core/util/FactoryRegistry.hpp>

#include <string>

namespace camp
{
class UserObject;
}
namespace fwTools
{
class Object;
}

namespace sight::core::reflection
{
namespace registry
{

typedef std::string KeyType;

typedef core::util::FactoryRegistry< SPTR(::camp::UserObject)(core::tools::Object*), KeyType > Type;

CORE_API SPTR(Type) get();

} // namespace registry
} // namespace sight::core::reflection
