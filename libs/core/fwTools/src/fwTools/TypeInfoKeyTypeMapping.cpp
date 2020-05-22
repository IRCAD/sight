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

#include "fwTools/TypeInfoKeyTypeMapping.hpp"
#include <typeinfo>


namespace fwTools
{

/**
 * @brief template specialization for defining KeyType mapping ( here std::type_info) with their value
 */
template<class T>
bool isMappingTypeInfoCommon(const std::type_info &key)
{
    return key == typeid( T );
}
// warning specialization from a partial specialization is not possible



// FIX UNSIGNED
template<>
bool isMapping<unsigned char>(const std::type_info &key)
{
    return isMappingTypeInfoCommon< unsigned char >(key);
}
template<>
bool isMapping<unsigned short>(const std::type_info &key)
{
    return isMappingTypeInfoCommon< unsigned short >(key);
}
template<>
bool isMapping<unsigned int>(const std::type_info &key)
{
    return isMappingTypeInfoCommon< unsigned int >(key);
}
template<>
bool isMapping<unsigned long>(const std::type_info &key)
{
    return isMappingTypeInfoCommon< unsigned long >(key);
}

// FIXED PRECISION SIGNED
template<>
bool isMapping<signed char>(const std::type_info &key)
{
    return isMappingTypeInfoCommon< signed char >(key);
}
template<>
bool isMapping<signed short>(const std::type_info &key)
{
    return isMappingTypeInfoCommon< signed short >(key);
}
template<>
bool isMapping<signed int>(const std::type_info &key)
{
    return isMappingTypeInfoCommon< signed int >(key);
}

template<>
bool isMapping<signed long>(const std::type_info &key)
{
    return isMappingTypeInfoCommon< signed long >(key);
}


// FLOATING PRECISION
template<>
bool isMapping<float>(const std::type_info &key)
{
    return isMappingTypeInfoCommon< float >(key);
}
template<>
bool isMapping<double>(const std::type_info &key)
{
    return isMappingTypeInfoCommon< double >(key);
}

// MISC
template<>
bool isMapping< std::string >(const std::type_info &key)
{
    return isMappingTypeInfoCommon< std::string >(key);
}


} // end namespace fwTools {
