/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

