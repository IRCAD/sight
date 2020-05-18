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

#ifndef __FWTOOLS_TYPEINFOKEYTYPEMAPPING_HPP__
#define __FWTOOLS_TYPEINFOKEYTYPEMAPPING_HPP__

#include <string>
#include <typeinfo>

#include "fwTools/TypeMapping.hpp"
#include "fwTools/config.hpp"

namespace fwTools
{

/**
 * @name TypeInfo mapping
 * @{
 */

/**
 * @brief       Template specialization for defining KeyType mapping ( here std::type_info) with their value.
 *
 * Header declare function already specialized while src provide definition
 *
 * @warning     isMapping does not need to be exported since fwTools Library is builded as static
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

} // namespace fwTools

#endif /*__FWTOOLS_TYPEINFOKEYTYPEMAPPING_HPP__*/
