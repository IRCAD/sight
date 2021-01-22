/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include "fwData/config.hpp"

#include <core/macros.hpp>

#include <string>

namespace fwData
{
class Object;
}

namespace fwData
{
namespace reflection
{

/**
 * @brief Returns the object designated by the given path within given object.
 *
 * @param object object to visit
 * @param path path of desired object
 * @param raiseException raise exceptions if set to true
 *
 * @throw ::fwData::reflection::exception::NullPointer
 * @throw ::fwData::reflection::exception::ObjectNotFound
 */
FWDATA_API SPTR(::fwData::Object) getObject( CSPTR(::fwData::Object) object,
                                             const std::string& path,
                                             bool raiseException = false);

/**
 * @see fwDataCamp.getObject
 */
template< class DATATYPE >
SPTR(DATATYPE) getObject( CSPTR(::fwData::Object) object,
                          const std::string& path,
                          bool raiseException = false )
{
    SPTR(::fwData::Object) subObject = getObject( object, path, raiseException );
    SPTR(DATATYPE) casteDdata        = ::std::dynamic_pointer_cast<DATATYPE>( subObject );
    return casteDdata;
}

} // namespace reflection
} // namespace fwData
