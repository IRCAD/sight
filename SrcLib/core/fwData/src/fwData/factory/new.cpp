/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/Object.hpp"

#include "fwData/factory/new.hpp"


namespace fwData
{

namespace factory
{

::fwData::Object::sptr New( const ::fwData::registry::KeyType & classname )
{
    return ::fwData::registry::get()->create(classname);
}

} // namespace factory

} // namespace fwData


