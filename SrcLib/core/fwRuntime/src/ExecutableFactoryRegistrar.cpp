/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/ExecutableFactoryRegistrar.hpp"

#include <cassert>

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/Runtime.hpp"



namespace fwRuntime
{



ExecutableFactoryRegistrar::ExecutableFactoryRegistrar( ::boost::shared_ptr< ExecutableFactory > factory )
{
    // Pre-condition
    assert( Bundle::getLoadingBundle() != 0 );

    // Retrieves the bundle that is currently loading.
     ::boost::shared_ptr< Bundle >  loadingBundle   ( Bundle::getLoadingBundle()    );
    Runtime                 * runtime       ( Runtime::getDefault()         );

    // Stores the factory into that bundle and the default runtime instance.
    loadingBundle->addExecutableFactory( factory );
    runtime->addExecutableFactory( factory );
}



} // namespace fwRuntime
