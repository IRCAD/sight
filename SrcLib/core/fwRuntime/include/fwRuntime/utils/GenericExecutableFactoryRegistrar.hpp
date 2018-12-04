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

#ifndef __FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORYREGISTRAR_HPP__
#define __FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORYREGISTRAR_HPP__

#include <string>

#include "fwRuntime/ExecutableFactoryRegistrar.hpp"
#include "fwRuntime/utils/GenericExecutableFactory.hpp"


namespace fwRuntime
{

namespace utils
{

/**
 * @brief   Defines a generic template executable factory registrar class.
 * @struct  GenericExecutableFactoryRegistrar
 * @date    2004-2009
 *
 */
template< typename E >
struct GenericExecutableFactoryRegistrar : public ExecutableFactoryRegistrar
{

    typedef GenericExecutableFactory< E >   FactoryType;

    /**
     * @brief   Constructor
     */
    GenericExecutableFactoryRegistrar(const std::string& type)
        : ExecutableFactoryRegistrar( std::shared_ptr< FactoryType >( new FactoryType(type) ) )
    {
    }

};


} // namespace utils

} // namespace fwRuntime


#endif //__FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORYREGISTRAR_HPP__
