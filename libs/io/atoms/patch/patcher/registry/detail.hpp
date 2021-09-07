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

#include "io/atoms/config.hpp"

#include <core/FactoryRegistry.hpp>
#include <core/macros.hpp>

#include <string>

namespace sight::io::atoms::patch
{

namespace patcher
{

class IPatcher;

namespace registry
{

typedef std::string KeyType;

typedef core::FactoryRegistry<SPTR(io::atoms::patch::patcher::IPatcher)(), KeyType> Type;

IO_ATOMS_API SPTR(Type) get();

} // namespace registry

} // namespace patcher

} // namespace sight::io::atoms::patch
