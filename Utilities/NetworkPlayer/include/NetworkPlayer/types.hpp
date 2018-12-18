/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __NETWORKPLAYER_TYPES_HPP__
#define __NETWORKPLAYER_TYPES_HPP__

#include <istream>
#include <boost/function.hpp>

namespace networkPlayer
{
/**
 * @struct PlayerFunction
 *
 * @tparam Socket Socket type to use
 *
 * @brief only a struct define a logger function type with a unknown type(like type alias functionnality in c++11).
 *
 * @see http://en.cppreference.com/w/cpp/language/type_alias
 */
template<typename Socket>
struct PlayerFunction
{
    typedef ::boost::function<void (std::istream&, Socket&)> FunctionType;
};
}

#endif // __NETWORKPLAYER_TYPES_HPP__