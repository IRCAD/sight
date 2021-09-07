/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/atoms/patch/patcher/factory/new.hpp"

#include "io/atoms/patch/patcher/IPatcher.hpp"

namespace sight::io::atoms::patch
{

namespace patcher
{

namespace factory
{

SPTR(io::atoms::patch::patcher::IPatcher) New(const io::atoms::patch::patcher::registry::KeyType& classname)
{
    return io::atoms::patch::patcher::registry::get()->create(classname);
}

} // namespace factory

} // namespace patcher

} // namespace sight::io::atoms::patch
