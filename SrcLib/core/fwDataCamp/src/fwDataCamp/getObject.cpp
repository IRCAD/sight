/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/Object.hpp>

#include "fwDataCamp/getObject.hpp"
#include "fwDataCamp/visitor/GetObject.hpp"
#include "fwDataCamp/exception/NullPointer.hpp"
#include "fwDataCamp/exception/ObjectNotFound.hpp"

namespace fwDataCamp
{

::fwData::Object::sptr getObject( ::fwData::Object::csptr object,
                                  const std::string& path,
                                  bool raiseException )
{
    SLM_FATAL_IF( "Path for desired object is empty.", path.empty() );
    SLM_FATAL_IF( "Path for desired object doesn't start with '@'.", path[0] != '@' );
    const std::string objectPath = path.substr( 1 );
    ::fwDataCamp::visitor::GetObject visitor( object, objectPath );
    ::fwData::Object::sptr subObject;
    try
    {
        subObject = visitor.get();
    }
    catch(const ::fwDataCamp::exception::NullPointer& np)
    {
        FW_FORWARD_EXCEPTION_IF(np, raiseException);
    }

    FW_RAISE_EXCEPTION_IF(
        ::fwDataCamp::exception::ObjectNotFound("Object '" + path + "' not found."),
        !visitor.objectsFound() && raiseException);

    return subObject;
}

} // namespace fwDataCamp

