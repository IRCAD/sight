/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef STRINGKEYTYPEMAPPING_HPP_
#define STRINGKEYTYPEMAPPING_HPP_

#include <string>

#include "fwTools/TypeMapping.hpp"
#include "fwTools/config.hpp"

namespace fwTools {

/**
 * @name    String type mapping
 * @{
 */
/**
 * @brief   Template specialization for defining KeyType mapping ( here std::string) with their value.
 *
 * Header declare function already specialized while src provide definition.
 *
 * @warning isMapping does not need to be exported since fwTools Library is builded as static.
 * @author  IRCAD (Research and Development Team).
 */

// FIX UNSIGNED
template<> FWTOOLS_API
bool isMapping<unsigned char>(const std::string &key);

template<> FWTOOLS_API
bool isMapping<unsigned short>(const std::string &key);

template<> FWTOOLS_API
bool isMapping<unsigned int>(const std::string &key);

template<> FWTOOLS_API
bool isMapping<unsigned long>(const std::string &key);


// FIXED PRECISION SIGNED
template<> FWTOOLS_API
bool isMapping<signed char>(const std::string &key);

template<> FWTOOLS_API
bool isMapping<signed short>(const std::string &key);

template<> FWTOOLS_API
bool isMapping<signed int>(const std::string &key);

template<> FWTOOLS_API
bool isMapping<signed long>(const std::string &key);




// FLOATING PRECISION
template<> FWTOOLS_API
bool isMapping<float>(const std::string &key);

template<> FWTOOLS_API
bool isMapping<double>(const std::string &key);


// MISC
template<> FWTOOLS_API
bool isMapping<std::string>(const std::string &key);
///@}

} // end namespace fwTools {

#endif /*STRINGKEYTYPEMAPPING_HPP_*/
