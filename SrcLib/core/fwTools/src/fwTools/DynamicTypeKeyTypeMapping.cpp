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

#include "fwTools/DynamicTypeKeyTypeMapping.hpp"


namespace fwTools
{
/*
 * @brief template specialization for defining KeyType mapping ( here DynamicType) with their value
 * @auhtor Vincent Agnus
 */


// FIX UNSIGNED
template<>
bool isMapping<unsigned char>(const DynamicType &key)
{
    return key.isType<unsigned char>();
}
template<>
bool isMapping<unsigned short>(const DynamicType &key)
{
    return key.isType<unsigned short>();
}
template<>
bool isMapping<unsigned int>(const DynamicType &key)
{
    return key.isType<unsigned int>();
}
template<>
bool isMapping<unsigned long>(const DynamicType &key)
{
    return key.isType<unsigned long>();
}

// FIXED PRECISION SIGNED
template<>
bool isMapping<signed char>(const DynamicType &key)
{
    return key.isType<signed char>();
}
template<>
bool isMapping<signed short>(const DynamicType &key)
{
    return key.isType<signed short>();
}
template<>
bool isMapping<signed int>(const DynamicType &key)
{
    return key.isType<signed int>();
}

template<>
bool isMapping<signed long>(const DynamicType &key)
{
    return key.isType<signed long>();
}


// FLOATING PRECISION
template<>
bool isMapping<float>(const DynamicType &key)
{
    return key.isType<float>();
}
template<>
bool isMapping<double>(const DynamicType &key)
{
    return key.isType<double>();
}

// MISC
template<>
bool isMapping<std::string>(const DynamicType &key)
{
    return key.isType<std::string>();
}



} // end namespace fwTools {

