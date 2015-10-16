/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
