/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWDATACAMP_GETOBJECT_HPP__
#define  __FWDATACAMP_GETOBJECT_HPP__

#include <string>


#include <fwCore/macros.hpp>

#include "fwDataCamp/config.hpp"

namespace fwData
{
class Object;
}

namespace fwDataCamp
{

/**
 * @brief Returns the object designated by the given path within given object.
 *
 * @param object object to visit
 * @param path path of desired object
 * @param raiseException raise exceptions if set to true
 *
 * @throw ::fwDataCamp::exception::NullPointer
 * @throw ::fwDataCamp::exception::ObjectNotFound
 */
FWDATACAMP_API SPTR(::fwData::Object) getObject( CSPTR(::fwData::Object) object,
                                                 const std::string & path,
                                                 bool raiseException = false);

/**
 * @see fwDataCamp.getObject
 */
template< class DATATYPE >
SPTR(DATATYPE) getObject( CSPTR(::fwData::Object) object,
                          const std::string & path,
                          bool raiseException = false )
{
    SPTR(::fwData::Object) subObject = getObject( object, path, raiseException );
    SPTR(DATATYPE) casteDdata        = ::std::dynamic_pointer_cast<DATATYPE>( subObject );
    return casteDdata;
}


} // namespace fwDataCamp

#endif // __FWDATACAMP_GETOBJECT_HPP__

