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

#ifndef __FWTOOLS_INTEGERTYPES_HPP__
#define __FWTOOLS_INTEGERTYPES_HPP__

#include <boost/mpl/vector.hpp>

namespace fwTools
{

typedef ::boost::mpl::vector<
        signed char, unsigned char,
        signed short, unsigned short,
        signed int, unsigned int
#ifndef DEBUG
        ,signed long, unsigned long
#endif
        >::type IntegerTypes;


} // end namespace fwTools

#endif //__FWTOOLS_INTEGERTYPES_HPP__
