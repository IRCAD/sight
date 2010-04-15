/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/TriangularMesh.hpp>

#include "io/IReader.hpp"
#include "io/IWriter.hpp"

#include "ioXML/Plugin.hpp"

#include "ioXML/FwXMLImageReaderService.hpp"
#include "ioXML/FwXMLImageWriterService.hpp"
#include "ioXML/FwXMLPatientDBReaderService.hpp"
#include "ioXML/FwXMLPatientDBWriterService.hpp"
#include "ioXML/FwXMLPatient2PatientDBWriterService.hpp"
#include <fwXML/ImageFileFormatService.hpp>


namespace ioXML
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("ioXML::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    /// For component based declaration of services
    REGISTER_SERVICE( ::io::IReader , ::ioXML::FwXMLImageReaderService , ::fwData::Image ) ;
    REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLImageWriterService , ::fwData::Image ) ;
    REGISTER_SERVICE( ::io::IReader , ::ioXML::FwXMLPatientDBReaderService , ::fwData::PatientDB ) ;
    REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLPatientDBWriterService , ::fwData::PatientDB ) ;
    REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLPatient2PatientDBWriterService , ::fwData::Patient ) ;


    if ( this->getBundle()->hasParameter("defaultImageWriter") )
    {
        std::string libwriter(  this->getBundle()->getParameterValue("defaultImageWriter") ) ;
        ::fwXML::ImageFileFormatService::setPreferedWriter( libwriter );
    }

}

void Plugin::stop() throw()
{}

} // namespace operators
