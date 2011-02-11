/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwPython/python.hpp>
#include <fwPython/bindings.hpp>

#include "uiPython/Plugin.hpp"

namespace uiPython
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::uiPython::Plugin");

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    ::fwPython::initialize();
    ::fwPython::initializeBindings();
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{}

} // namespace uiPython
