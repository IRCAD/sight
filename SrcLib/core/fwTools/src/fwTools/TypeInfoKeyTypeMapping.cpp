/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/TypeInfoKeyTypeMapping.hpp"
#include <typeinfo>


namespace fwTools {

/**
 * @brief template specialization for defining KeyType mapping ( here std::type_info) with their value
 * @auhtor Vincent Agnus
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
