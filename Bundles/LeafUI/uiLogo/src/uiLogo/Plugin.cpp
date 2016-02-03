/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiLogo/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwServices/macros.hpp>

namespace uiLogo
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::uiLogo::Plugin");

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
