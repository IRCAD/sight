/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef DYNAMICTYPEKEYTYPEMAPPING_HPP_
#define DYNAMICTYPEKEYTYPEMAPPING_HPP_

#include "fwTools/DynamicType.hpp"

#include "fwTools/TypeMapping.hpp"
#include "fwTools/config.hpp"


namespace fwTools {


/**
 * @name	Dynamic key type mapping
 * @{
 */

/**
 * @brief	Template specialization for defining KeyType mapping ( here DynamicType) with their value.
 *
 * Header declare function already specialized while src provide definition
 * @warning	isMapping does not need to be exported since fwTools Library is builded as static
 */
// FIX UNSIGNED
template<> FWTOOLS_API
bool isMapping<unsigned char>(const DynamicType &key);

template<> FWTOOLS_API
bool isMapping<unsigned short>(const DynamicType &key);

template<> FWTOOLS_API
bool isMapping<unsigned int>(const DynamicType &key);

template<> FWTOOLS_API
bool isMapping<unsigned long>(const DynamicType &key);


// FIXED PRECISION SIGNED
template<> FWTOOLS_API
bool isMapping<signed char>(const DynamicType &key);

template<> FWTOOLS_API
bool isMapping<signed short>(const DynamicType &key);

template<> FWTOOLS_API
bool isMapping<signed int>(const DynamicType &key);

template<> FWTOOLS_API
bool isMapping<signed long>(const DynamicType &key);



// FLOATING PRECISION
template<> FWTOOLS_API
bool isMapping<float>(const DynamicType &key);

template<> FWTOOLS_API
bool isMapping<double>(const DynamicType &key);


// MISC
template<> FWTOOLS_API
bool isMapping<std::string>(const DynamicType &key);

///@}


} //end namespace fwTools {


#endif /*STRINGKEYTYPEMAPPING_HPP_*/
