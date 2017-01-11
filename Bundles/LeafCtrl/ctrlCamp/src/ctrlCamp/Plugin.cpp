/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlCamp/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace ctrlCamp
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::ctrlCamp::Plugin");

Plugin::~Plugin() throw()
{
}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
}

void Plugin::stop() throw()
{
}

} // namespace ctrlCamp
