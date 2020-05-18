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

#ifndef __FWTOOLS_DYNAMICTYPEKEYTYPEMAPPING_HPP__
#define __FWTOOLS_DYNAMICTYPEKEYTYPEMAPPING_HPP__

#include "fwTools/DynamicType.hpp"

#include "fwTools/TypeMapping.hpp"
#include "fwTools/config.hpp"


namespace fwTools
{


/**
 * @name    Dynamic key type mapping
 * @{
 */

/**
 * @brief   Template specialization for defining KeyType mapping ( here DynamicType) with their value.
 *
 * Header declare function already specialized while src provide definition
 * @warning isMapping does not need to be exported since fwTools Library is builded as static
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


} //end namespace fwTools


#endif /*__FWTOOLS_DYNAMICTYPEKEYTYPEMAPPING_HPP__*/
