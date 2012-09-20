/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCamp/UserObject.hpp"
#include "fwCamp/factory/new.hpp"

namespace fwCamp
{
namespace factory
{

SPTR( ::camp::UserObject ) New( const ::fwCamp::registry::KeyType & classname, ::fwTools::Object *object )
{
    return ::fwCamp::registry::get()->create(classname, object);
}

} // namespace factory
} // namespace fwCamp


