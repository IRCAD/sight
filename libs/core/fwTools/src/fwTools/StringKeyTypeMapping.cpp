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

#include "fwTools/StringKeyTypeMapping.hpp"

namespace fwTools
{

/*
 * @brief template specialization for defining KeyType mapping ( here std::string) with their value
 * @auhtor Vincent Agnus
 */


// FIX UNSIGNED
template<>
bool isMapping<unsigned char>(const std::string &key)
{
    return key=="unsigned char";
}
template<>
bool isMapping<unsigned short>(const std::string &key)
{
    return key=="unsigned short";
}
template<>
bool isMapping<unsigned int>(const std::string &key)
{
    return key=="unsigned int";
}
template<>
bool isMapping<unsigned long>(const std::string &key)
{
    return key=="unsigned long";
}

// FIXED PRECISION SIGNED
template<>
bool isMapping<signed char>(const std::string &key)
{
    return key=="signed char";
}
template<>
bool isMapping<signed short>(const std::string &key)
{
    return key=="signed short";
}
template<>
bool isMapping<signed int>(const std::string &key)
{
    return key=="signed int";
}

template<>
bool isMapping<signed long>(const std::string &key)
{
    return key=="signed long";
}


// FLOATING PRECISION
template<>
bool isMapping<float>(const std::string &key)
{
    return key=="float";
}
template<>
bool isMapping<double>(const std::string &key)
{
    return key=="double";
}

// MISC
template<>
bool isMapping<std::string>(const std::string &key)
{
    return key=="std::string";
}



} // end namespace fwTools {

