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

#include "atoms/conversion/mapper/factory/new.hpp"

#include "atoms/conversion/mapper/Base.hpp"

namespace sight::atoms::conversion
{
namespace mapper
{
namespace factory
{

SPTR(atoms::conversion::mapper::Base) New( const atoms::conversion::mapper::registry::KeyType& classname )
{
    return atoms::conversion::mapper::registry::get()->create(classname);
}

} // namespace factory
} // namespace mapper
} // namespace sight::atoms::conversion
