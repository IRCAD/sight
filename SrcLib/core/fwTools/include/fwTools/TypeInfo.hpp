/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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
 * @brief   Purpose: offer a first-class, comparable wrapper over std::type_info ( but copy construcible )
 * @class   TypeInfo
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 * Code imported from Loki-Library and adapted for FW4SPL
 */

class FWTOOLS_CLASS_API TypeInfo
{
public:

    /// default constructor
    FWTOOLS_API TypeInfo(); // needed for containers

    /// copy constructor
    FWTOOLS_API TypeInfo(const std::type_info&); // non-explicit

    /**
     * @brief copy method
     * @param[in] the TypeInfo to copy
     * @return the copied TypeInfo
     */
    FWTOOLS_API TypeInfo &operator=(const TypeInfo&);

    /**
     * @brief access for the wrapped std::type_info
     * @return a const reference on the embedded  std::type_info
     */
    FWTOOLS_API const std::type_info& type_info() const;

    /**
     * @brief cast operator TypeInfo -> std::type_info
     * @return a const reference on the embedded  std::type_info
     */
    FWTOOLS_API operator const std::type_info&() const
    {
        return *pInfo_;
    }


    // Compatibility functions
    /**
     * @brief compatibility functions ( std::type_info like)
     * @return true iff TypeInfo is "before" an other, ( ordering is defined as std::type_info::before() )
     */
    FWTOOLS_API bool before(const TypeInfo& rhs) const;

    /**
     * @brief compatibility functions ( std::type_info like)
     * @return the lexical name of the typeinfo ( = td::type_info::name() )
     */
    FWTOOLS_API const char* name() const;

private:

    /// std::type_info are not copiable but its pointer yes and typeid always return same reference !!!
    const std::type_info* pInfo_;
};


// COMPARISON OPERATOR
/**
 * @name Comparison operators for TypeInfo
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
