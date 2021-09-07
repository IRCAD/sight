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

#include "io/base/reader/factory/new.hpp"

#include "io/base/reader/IObjectReader.hpp"

namespace sight::io::base
{

namespace reader
{

namespace factory
{

//------------------------------------------------------------------------------

io::base::reader::IObjectReader::sptr New(const io::base::reader::registry::KeyType& classname)
{
    return io::base::reader::registry::get()->create(classname);
}

} // namespace factory

} // namespace reader

} // namespace sight::io::base
