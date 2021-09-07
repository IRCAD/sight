/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "core/reflection/registry/detail.hpp"
#include <core/macros.hpp>

#include <string>

namespace sight::core::reflection
{

namespace factory
{

CORE_API SPTR(::camp::UserObject) New(
    const core::reflection::registry::KeyType& classname,
    core::tools::Object* object
);

template<class CLASSNAME>
SPTR(::camp::UserObject)  New(core::tools::Object* object)
{
    SPTR(::camp::UserObject) obj = std::make_shared< ::camp::UserObject>(*(dynamic_cast<CLASSNAME*>(object)));
    return obj;
}

} // namespace factory

} // namespace sight::core::reflection
