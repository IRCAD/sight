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

#pragma once

#include "io/atoms/config.hpp"
#include "io/atoms/filter/registry/detail.hpp"

#include <string>

namespace sight::io::atoms::filter
{

class IFilter;

namespace factory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  New();

/**
 * @brief Key class used to restrict access to Object construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template<typename CLASSNAME>
friend SPTR(CLASSNAME) io::atoms::filter::factory::New();

Key()
{
}
};

IO_ATOMS_API SPTR(io::atoms::filter::IFilter) New(
    const io::atoms::filter::registry::KeyType& classname
);

template<class CLASSNAME>
SPTR(CLASSNAME)  New()
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>(Key());

    return obj;
}

} // namespace factory

} // namespace sight::io::atoms::filter
