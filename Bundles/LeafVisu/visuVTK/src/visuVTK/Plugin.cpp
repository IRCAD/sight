/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <fwRender/IRender.hpp>
#include <fwRenderVTK/VtkRenderService.hpp>
#include <fwRenderVTK/vtk/Instantiator.hpp>

#include <fwData/Composite.hpp>

#include "visuVTK/Plugin.hpp"

namespace visuVTK
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("visuVTK::Plugin");


Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    SLM_TRACE_FUNC();
    //REGISTER_SERVICE( ::fwRender::IRender , ::fwRenderVTK::VtkRenderService , ::fwData::Composite) ;

    ::fwRenderVTK::vtk::Instantiator::ClassInitialize();
}

void Plugin::stop() throw()
{
    ::fwRenderVTK::vtk::Instantiator::ClassFinalize();
}

} // namespace operators
