/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/filesystem/operations.hpp>

#include <io/IReader.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwCore/base.hpp>
#include <fwServices/macros.hpp>

#include <fwDataIO/writer/TriangularMeshWriter.hpp>

#include "ioData/TriangularMeshWriterService.hpp"

REGISTER_SERVICE( ::io::IWriter , ::ioData::TriangularMeshWriterService , ::fwData::TriangularMesh ) ;

namespace ioData
{

TriangularMeshWriterService::TriangularMeshWriterService():
        m_filename (""),
        m_bServiceIsConfigured(false)
{
}

//-----------------------------------------------------------------------------

void TriangularMeshWriterService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream << std::endl << " Triangular Mesh writer" ;
}

//-----------------------------------------------------------------------------

std::vector< std::string > TriangularMeshWriterService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".trian");
    return extensions ;
}

//-----------------------------------------------------------------------------

TriangularMeshWriterService::~TriangularMeshWriterService() throw()
{}

//------------------------------------------------------------------------------

void TriangularMeshWriterService::configuring( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getValue() ;
        OSLM_INFO( "TriangularMeshWriterService::configure filename: " << filename );
        m_filename = ::boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = true;
    }
}

//------------------------------------------------------------------------------

void TriangularMeshWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a TrianMesh file");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TrianMesh","*.trian");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        m_filename = result->getPath();
        m_bServiceIsConfigured = true;
        _sDefaultPath = m_filename.branch_path();
    }

}

//------------------------------------------------------------------------------

void TriangularMeshWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if(m_bServiceIsConfigured)
    {
        // Retrieve object
        ::fwData::TriangularMesh::sptr mesh = this->getObject< ::fwData::TriangularMesh >( );
        assert( mesh ) ;

        ::fwDataIO::writer::TriangularMeshWriter writer;
        writer.setObject( mesh );
        writer.setFile(m_filename);
        writer.write();
    }
}

//------------------------------------------------------------------------------

}
