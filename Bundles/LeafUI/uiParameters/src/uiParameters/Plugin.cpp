/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiParameters/Plugin.hpp"

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwServices/macros.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace uiParameters
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::uiParameters::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{
}

//-----------------------------------------------------------------------------

void Plugin::start() throw( ::fwRuntime::RuntimeException )
{
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
}

//-----------------------------------------------------------------------------

} // namespace operators
