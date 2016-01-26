/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCAMP_FACTORY_NEW_HPP__
#define __FWCAMP_FACTORY_NEW_HPP__

#include <string>


#include <fwCore/macros.hpp>

#include "fwCamp/config.hpp"
#include "fwCamp/registry/detail.hpp"

namespace fwCamp
{
namespace factory
{

FWCAMP_API SPTR( ::camp::UserObject ) New( const ::fwCamp::registry::KeyType & classname, ::fwTools::Object *object);

template<class CLASSNAME > SPTR( ::camp::UserObject )  New(::fwTools::Object *object)
{
    SPTR(::camp::UserObject) obj = std::make_shared< ::camp::UserObject >(*(dynamic_cast<CLASSNAME*>( object )));
    return obj;
}

} // namespace factory
} // namespace fwCamp

#endif /* __FWCAMP_FACTORY_NEW_HPP__ */


