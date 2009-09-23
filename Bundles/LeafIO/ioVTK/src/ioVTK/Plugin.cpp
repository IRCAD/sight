/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>
#include <io/IReader.hpp>
#include <fwData/Camera.hpp>
#include <fwData/Model.hpp>
#include <fwData/Spline.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/PatientDB.hpp>

#include <fwServices/macros.hpp>

#include "ioVTK/Plugin.hpp"

#include "ioVTK/MaxMeshReaderService.hpp"
#include "ioVTK/ReconstructionWriterService.hpp"
#include "ioVTK/ImageWriterService.hpp"
#include "ioVTK/ImageReaderService.hpp"
#include "ioVTK/VtkPatientDBReaderService.hpp"
#include "ioVTK/MeshReaderService.hpp"

namespace ioVTK
{
using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("ioVTK::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
	/// Loading dynamic library at runtime not sufficient to register services in factory: one has to explicitely declare this
//	REGISTER_SERVICE( ::tracker::ITracker , ::magnetTracker::Tracker , ::fwData::Spline ) ;
	REGISTER_SERVICE( ::io::IReader , ::ioVTK::MaxMeshReaderService , ::fwData::Model ) ;
	REGISTER_SERVICE( ::io::IReader , ::ioVTK::MeshReaderService , ::fwData::TriangularMesh ) ;
	REGISTER_SERVICE( ::io::IReader , ::ioVTK::ImageReaderService , ::fwData::Image ) ;
	REGISTER_SERVICE( ::io::IReader , ::ioVTK::VtkPatientDBReaderService , ::fwData::PatientDB ) ;
	REGISTER_SERVICE( ::io::IWriter , ::ioVTK::ReconstructionWriterService , ::fwData::Acquisition ) ;
	REGISTER_SERVICE( ::io::IWriter , ::ioVTK::ImageWriterService , ::fwData::Image ) ;


}

void Plugin::stop() throw()
{}

} // namespace operators
