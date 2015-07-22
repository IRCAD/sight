/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "ctrlPointSelection/Plugin.hpp"

namespace ctrlPointSelection
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::ctrlPointSelection::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{
}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
}

//-----------------------------------------------------------------------------

} // namespace ctrlPointSelection

