/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef FWTOOLS_TYPEINFO
#define FWTOOLS_TYPEINFO

#include <typeinfo>
#include "fwTools/config.hpp"
#include <cassert>

namespace fwTools
{
/**
 * @brief	Purpose: offer a first-class, comparable wrapper over std::type_info ( but copy construcible )
 * @class  	TypeInfo
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @todo	Complete doxygen
 *
 * Code imported from Loki-Library and adapted for FW4SPL
 */

class FWTOOLS_CLASS_API TypeInfo
{
public:
	// Constructors
	FWTOOLS_API TypeInfo(); // needed for containers
	FWTOOLS_API TypeInfo(const std::type_info&); // non-explicit

	/// Copy method
	FWTOOLS_API TypeInfo &operator=(const TypeInfo&);

	/// Access for the wrapped std::type_info
	FWTOOLS_API const std::type_info& type_info() const;

	FWTOOLS_API operator const std::type_info&() const
	{
		return *pInfo_;
	}


	// Compatibility functions
	FWTOOLS_API bool before(const TypeInfo& rhs) const;
	FWTOOLS_API const char* name() const;

private:

	/// std::type_info are not copiable but its pointer yes and typeid always return same reference !!!
	const std::type_info* pInfo_;
};


// COMPARISON OPERATOR
/**
 * @name Compariton operator for TypeInfo
 * @{
 */
FWTOOLS_API bool operator==(const TypeInfo& lhs, const TypeInfo& rhs);
FWTOOLS_API bool operator<(const TypeInfo& lhs, const TypeInfo& rhs);
FWTOOLS_API bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs);
FWTOOLS_API bool operator>(const TypeInfo& lhs, const TypeInfo& rhs);
FWTOOLS_API bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs);
FWTOOLS_API bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs);
///@}



}

#endif /* FWTOOLS_TYPEINFO_ */
