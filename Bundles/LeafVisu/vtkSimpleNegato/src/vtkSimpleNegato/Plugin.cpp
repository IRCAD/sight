/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>

#include <fwRender/IRender.hpp>

#include <vtkSimpleNegato/RendererService.hpp>

#include <fwData/Image.hpp>

#include "vtkSimpleNegato/Plugin.hpp"

namespace vtkSimpleNegato
{
using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("vtkSimpleNegato::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{

	REGISTER_SERVICE( ::fwRender::IRender , ::vtkSimpleNegato::RendererService, ::fwData::Image) ;

}

void Plugin::stop() throw()
{}

} // namespace operators
