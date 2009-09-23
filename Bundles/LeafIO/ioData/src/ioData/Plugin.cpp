/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>
#include <io/IReader.hpp>
#include <fwData/Camera.hpp>
#include <fwData/Spline.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include "ioData/Plugin.hpp"

#include "ioData/CameraReaderService.hpp"
#include "ioData/SplineReaderService.hpp"
#include "ioData/TriangularMeshReaderService.hpp"
#include "ioData/DefaultCompositeObjectReaderService.hpp"
#include "ioData/CompositeReaderService.hpp"
#include "ioData/TransformationMatrix3DReaderService.hpp"
#include "ioData/TransformationMatrix3DWriterService.hpp"
#include "ioData/TriangularMeshWriterService.hpp"

namespace ioData
{
using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("ioData::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
	/// Loading dynamic library at runtime not sufficient to register services in factory: one has to explicitely declare this
	REGISTER_SERVICE( ::io::IReader , ::ioData::CameraReaderService , ::fwData::Camera ) ;
	REGISTER_SERVICE( ::io::IReader , ::ioData::SplineReaderService , ::fwData::Spline ) ;
	REGISTER_SERVICE( ::io::IReader , ::ioData::TriangularMeshReaderService , ::fwData::TriangularMesh ) ;

	/// For component based declaration of services
	REGISTER_SERVICE( ::io::IReader , ::ioData::DefaultCompositeObjectReaderService , ::fwData::ProcessObject ) ;
	REGISTER_SERVICE( ::io::IReader , ::ioData::CompositeReaderService , ::fwData::Composite ) ;
	REGISTER_SERVICE( ::io::IReader , ::ioData::TransformationMatrix3DReaderService , ::fwData::TransformationMatrix3D ) ;

	REGISTER_SERVICE( ::io::IWriter , ::ioData::TransformationMatrix3DWriterService , ::fwData::TransformationMatrix3D ) ;
	REGISTER_SERVICE( ::io::IWriter , ::ioData::TriangularMeshWriterService , ::fwData::TriangularMesh ) ;

}

void Plugin::stop() throw()
{}

} // namespace operators
