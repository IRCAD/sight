/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTK/Plugin.hpp"

#include <fwRender/IRender.hpp>

#include <fwRenderVTK/SRender.hpp>
#include <fwRenderVTK/vtk/Instantiator.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>

namespace visuVTK
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::visuVTK::Plugin");

/// Register service ::fwRenderVTK::SRender into fw4spl service registry
fwServicesRegisterMacro( ::fwRender::IRender, ::fwRenderVTK::SRender );

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    ::fwRenderVTK::vtk::Instantiator::ClassInitialize();
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    ::fwRenderVTK::vtk::Instantiator::ClassFinalize();
}

} // namespace visuVTK
