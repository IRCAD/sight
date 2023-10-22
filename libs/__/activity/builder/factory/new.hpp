/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "activity/builder/registry/detail.hpp"
#include "activity/config.hpp"

#include <string>

namespace sight::activity::builder
{

class base;

namespace factory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  make();

ACTIVITY_API SPTR(sight::activity::builder::base) make(const activity::builder::registry::key_t& _classname);

template<class CLASSNAME>
SPTR(CLASSNAME)  make()
{
    SPTR(CLASSNAME) builder = std::make_shared<CLASSNAME>();
    return builder;
}

} // namespace factory

} // namespace sight::activity::builder
