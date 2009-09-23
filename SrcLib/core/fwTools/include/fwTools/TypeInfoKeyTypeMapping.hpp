/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef TYPEINFOGKEYTYPEMAPPING_HPP_
#define TYPEINFOGKEYTYPEMAPPING_HPP_

#include <string>
#include <typeinfo>

#include "fwTools/TypeMapping.hpp"
#include "fwTools/config.hpp"

namespace fwTools {

/**
 * @name TypeInfo mapping
 * @{
 */

/**
 * @brief 		Template specialization for defining KeyType mapping ( here std::type_info) with their value.
 *
 * Header declare function already specialized while src provide definition
 *
 * @warning 	isMapping does not need to be exported since fwTools Library is builded as static
 */

// FIX UNSIGNED
template<> FWTOOLS_API
bool isMapping<unsigned char>(const std::type_info &key);

template<> FWTOOLS_API
bool isMapping<unsigned short>(const std::type_info &key);

template<> FWTOOLS_API
bool isMapping<unsigned int>(const std::type_info &key);

template<> FWTOOLS_API
bool isMapping<unsigned long>(const std::type_info &key);


// FIXED PRECISION SIGNED
template<> FWTOOLS_API
bool isMapping<signed char>(const std::type_info &key);

template<> FWTOOLS_API
bool isMapping<signed short>(const std::type_info &key);

template<> FWTOOLS_API
bool isMapping<signed int>(const std::type_info &key);

template<> FWTOOLS_API
bool isMapping<signed long>(const std::type_info &key);




// FLOATING PRECISION
template<> FWTOOLS_API
bool isMapping<float>(const std::type_info &key);

template<> FWTOOLS_API
bool isMapping<double>(const std::type_info &key);


// MISC
template<> FWTOOLS_API
bool isMapping<std::string>(const std::type_info &key);
///@}

} // namespace fwTools {

#endif /*TYPEINFOGKEYTYPEMAPPING_HPP_*/
