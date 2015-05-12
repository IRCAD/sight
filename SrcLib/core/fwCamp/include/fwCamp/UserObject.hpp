/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

