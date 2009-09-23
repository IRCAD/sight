/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>


#include "io/IReader.hpp"
#include "io/IWriter.hpp"
#include "io/Plugin.hpp"


namespace io
{
using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("io::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{

}

void Plugin::stop() throw()
{}

} // namespace operators
