/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/StringKeyTypeMapping.hpp"

namespace fwTools {

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

