/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwCore/spyLog.hpp>

#include <vtkInstantiator.h>

extern vtkObject* vtkInstantiatorFixedInteractorStyleNew();

namespace visuVTKARAdaptor
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("visuVTKARAdaptor::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    SLM_TRACE_FUNC();
    vtkInstantiator::RegisterInstantiator("FixedInteractorStyle", vtkInstantiatorFixedInteractorStyleNew);
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    SLM_TRACE_FUNC();
    vtkInstantiator::UnRegisterInstantiator("FixedInteractorStyle", vtkInstantiatorFixedInteractorStyleNew);
}

//-----------------------------------------------------------------------------

} // namespace visuVTKARAdaptor
