/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWMEMORY_POLICY_FACTORY_NEW_HPP__
#define __FWMEMORY_POLICY_FACTORY_NEW_HPP__

#include "fwMemory/config.hpp"
#include "fwMemory/policy/registry/detail.hpp"

#include <string>

namespace fwMemory
{

class GuiBaseObject;

namespace policy
{
namespace factory
{

template<class CLASSNAME > SPTR( CLASSNAME )  New();

FWMEMORY_API SPTR( ::fwMemory::IPolicy ) New( const ::fwMemory::policy::registry::KeyType & classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) policy = std::make_shared< CLASSNAME >();
    return policy;
}

} // namespace factory
} // namespace policy
} // namespace fwMemory

#endif /* __FWMEMORY_POLICY_FACTORY_NEW_HPP__ */

