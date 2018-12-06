/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#ifndef  __FWATOMS_OBJECT_HXX__
#define  __FWATOMS_OBJECT_HXX__

#include "fwAtoms/config.hpp"
#include "fwAtoms/Base.hpp"
#include "fwAtoms/factory/new.hpp"

#ifndef __FWATOMS_OBJECT_HPP__
#error fwAtoms/Object.hpp not included
#endif

namespace fwAtoms
{

template < typename OBJECT_TYPE >
SPTR(OBJECT_TYPE) Object::getAttribute(const std::string& key) const
{
    ::fwAtoms::Base::sptr base = this->getAttribute(key);
    return std::dynamic_pointer_cast< OBJECT_TYPE >(base);
}

}

#endif /*  __FWATOMS_OBJECT_HXX__ */

