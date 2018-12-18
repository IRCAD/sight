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

#ifndef __FWCAMP_USEROBJECT_HPP__
#define __FWCAMP_USEROBJECT_HPP__

#include <camp/userobject.hpp>

#include <fwCore/macros.hpp>

#include "fwCamp/factory/new.hpp"
#include "fwCamp/registry/detail.hpp"

namespace fwCamp
{

/**
 * @brief Helper to register and manage the conversion between an fwData::Object and a camp::userObject
 */
template<typename T>
struct UserObjectRegistrar
{
    /**
     * @brief Register an object of type name in factory.
     */
    UserObjectRegistrar( const std::string &name )
    {
        ::fwCamp::registry::get()->addFactory(name, &::fwCamp::factory::New<T>);
    }
};

}

#endif //__FWCAMP_USEROBJECT_HPP__

