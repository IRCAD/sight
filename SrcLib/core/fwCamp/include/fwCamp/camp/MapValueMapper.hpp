/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __FWCAMP_CAMP_MAPVALUEMAPPER_HPP__
#define __FWCAMP_CAMP_MAPVALUEMAPPER_HPP__

#include "fwCamp/camp/customtype.hpp"
#include "fwCamp/camp/traits.hpp"

#include <camp/valuemapper.hpp>

namespace camp_ext
{

template <typename T>
struct ValueMapper<T, typename boost::enable_if_c<camp::isMapping<T>::value>::type>
{
    static const int type = camp::mappingType;
};

}  // namespace camp_ext

#endif /* __FWCAMP_CAMP_MAPVALUEMAPPER_HPP__ */
