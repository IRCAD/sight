/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "atoms/Base.hpp"
#include "atoms/config.hpp"
#include "atoms/factory/new.hpp"

namespace sight::atoms
{

template < typename OBJECT_TYPE >
SPTR(OBJECT_TYPE) Object::getAttribute(const std::string& key) const
{
    atoms::Base::sptr base = this->getAttribute(key);
    return std::dynamic_pointer_cast< OBJECT_TYPE >(base);
}

}
