/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>



#include "opVTKMesh/action/VTKMeshCreation.hpp"
#include "opVTKMesh/Plugin.hpp"

namespace opVTKMesh
{

//-----------------------------------------------------------------------------

using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("::opVTKMesh::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------

void Plugin::start() throw( ::fwRuntime::RuntimeException )
{
	REGISTER_SERVICE( ::gui::action::IAction , ::opVTKMesh::action::VTKMeshCreation , ::fwTools::Object ) ;
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{}

//-----------------------------------------------------------------------------

} // namespace operators
