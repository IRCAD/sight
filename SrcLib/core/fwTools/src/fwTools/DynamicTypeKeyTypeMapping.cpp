/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/DynamicTypeKeyTypeMapping.hpp"


namespace fwTools {
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

