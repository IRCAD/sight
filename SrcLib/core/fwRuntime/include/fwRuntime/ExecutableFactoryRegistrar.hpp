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

#ifndef __FWRUNTIME_EXECUTABLEFACTORYREGISTRAR_HPP__
#define __FWRUNTIME_EXECUTABLEFACTORYREGISTRAR_HPP__


#include "fwRuntime/config.hpp"

#include <memory>

namespace fwRuntime
{



struct ExecutableFactory;



/**
 * @brief   Defines an executable factory registrar class.
 * @struct  ExecutableFactoryRegistrar
 *
 * An instance of this class is responsible for the registration of an
 * executable factory instance in the runtime environment. The factory instace
 * gets registered as soon as a registrar instance gets created.
 * That's why this class is design to be instanciated as static object.
 *
 * @date    2004-2009
 *
 */
struct ExecutableFactoryRegistrar
{

    /**
     * @brief       Constructor
     *
     * @param[in]   factory a shared pointer to an executable factory instance to manage
     * @pre         Loading bundle exists.
     */
    FWRUNTIME_API ExecutableFactoryRegistrar( std::shared_ptr< ExecutableFactory > factory );

};


} // namespace fwRuntime



#endif //__FWRUNTIME_EXECUTABLEFACTORYREGISTRAR_HPP__
