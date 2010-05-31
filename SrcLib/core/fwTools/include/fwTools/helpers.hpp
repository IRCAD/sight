/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_HELPERS_HPP__
#define __FWTOOLS_HELPERS_HPP__

#include "fwTools/Object.hpp"

namespace fwTools {

template< typename DATA_SPTR_TYPE >
void getFieldFromObject(
        DATA_SPTR_TYPE &field,
        ::fwTools::Object::sptr obj,
        std::string fieldId,
        DATA_SPTR_TYPE defaultValue
        )
{
    field = getFieldFromObject(obj, fieldId, defaultValue);
}


template< typename DATA_SPTR_TYPE >
DATA_SPTR_TYPE getFieldFromObject(
        ::fwTools::Object::sptr obj,
        std::string fieldId,
        DATA_SPTR_TYPE defaultValue
        )
{

    if( obj->getFieldSize( fieldId ) )
    {
        OSLM_ASSERT("Field '"<< fieldId <<"' size > 1 ("<< obj->getFieldSize( fieldId ) << ")", obj->getFieldSize( fieldId ) == 1);
        return obj->getFieldSingleElement< typename DATA_SPTR_TYPE::element_type >( fieldId );
    }
    else
    {
        return defaultValue;
    }

}

} //end namespace fwTools

#endif // __FWTOOLS_HELPERS_HPP__
