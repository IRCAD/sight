/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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


