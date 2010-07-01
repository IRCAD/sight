/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>

#include "visuVTKAdaptor/NegatoWindower.hpp"
#include "visuVTKAdaptor/Plugin.hpp"


namespace visuVTKAdaptor
{
using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("visuVTKAdaptor::Plugin");


Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
	REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::NegatoWindower, ::visuVTKAdaptor::NegatoOneSlice ) ;
}

void Plugin::stop() throw()
{}

} // namespace operators
