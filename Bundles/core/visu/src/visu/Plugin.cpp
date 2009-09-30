/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "visu/Plugin.hpp"

namespace visu
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar< Plugin > registrar ("::visu::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{}

void Plugin::stop() throw()
{}

} // namespace visu
