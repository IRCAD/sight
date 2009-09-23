/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/EmptyPlugin.hpp"
#include "fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp"


namespace fwRuntime
{


void EmptyPlugin::start() throw(::fwRuntime::RuntimeException)
{}


void EmptyPlugin::stop() throw()
{}


} // namespace fwRuntime
