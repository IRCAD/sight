/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>

#include <fwRender/IRender.hpp>

#include <vtkSimpleMesh/RendererService.hpp>

#include <fwData/TriangularMesh.hpp>

#include "vtkSimpleMesh/Plugin.hpp"

namespace vtkSimpleMesh
{
using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("vtkSimpleMesh::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{

	REGISTER_SERVICE( ::fwRender::IRender , ::vtkSimpleMesh::RendererService , ::fwData::TriangularMesh) ;

}

void Plugin::stop() throw()
{}

} // namespace operators
