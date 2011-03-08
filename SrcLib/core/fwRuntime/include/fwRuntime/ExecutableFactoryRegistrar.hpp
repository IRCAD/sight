/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_EXECUTABLEFACTORYREGISTRAR_HPP_
#define _FWRUNTIME_EXECUTABLEFACTORYREGISTRAR_HPP_

#include <boost/shared_ptr.hpp>

#include "fwRuntime/config.hpp"



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
 * @author  IRCAD (Research and Development Team).
 */
struct ExecutableFactoryRegistrar
{

    /**
     * @brief       Constructor
     *
     * @param[in]   factory a shared pointer to an executable factory instance to manage
     * @pre         Loading bundle exists.
     */
    FWRUNTIME_API ExecutableFactoryRegistrar( ::boost::shared_ptr< ExecutableFactory > factory );

};


} // namespace fwRuntime



#endif //_FWRUNTIME_EXECUTABLEFACTORYREGISTRAR_HPP_
