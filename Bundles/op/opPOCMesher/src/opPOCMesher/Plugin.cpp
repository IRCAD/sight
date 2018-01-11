/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwServices/macros.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "opPOCMesher/Plugin.hpp"

namespace opPOCMesher
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::opPOCMesher::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//-----------------------------------------------------------------------------

} // namespace operators
