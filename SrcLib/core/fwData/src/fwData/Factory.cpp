/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Factory.hpp>

#include "fwData/Factory.hpp"

namespace fwData
{

::fwData::Object::sptr Factory::New(const std::string &classname)
{
    ::fwData::Object::sptr newObject = ::fwData::Object::dynamicCast( ::fwTools::Factory::New(classname) );
    OSLM_ASSERT("Sorry, this object is not a ::fwData::Object.", newObject );
    return newObject;
}

} // namespace fwData
