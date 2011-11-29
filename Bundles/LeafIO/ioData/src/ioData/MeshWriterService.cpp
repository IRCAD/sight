/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/macros.hpp>

#include <fwData/Mesh.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwDataIO/writer/MeshWriter.hpp>

#include "ioData/MeshWriterService.hpp"

REGISTER_SERVICE( ::io::IWriter , ::ioData::MeshWriterService , ::fwData::Mesh ) ;

namespace ioData
{

MeshWriterService::MeshWriterService():
        m_filename (""),
        m_bServiceIsConfigured(false)
{
}

//-----------------------------------------------------------------------------

void MeshWriterService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream << std::endl << " Mesh writer" ;
}

//-----------------------------------------------------------------------------

std::vector< std::string > MeshWriterService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".trian");
    return extensions ;
}

//-----------------------------------------------------------------------------

MeshWriterService::~MeshWriterService() throw()
{}

//------------------------------------------------------------------------------

void MeshWriterService::configuring( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getValue() ;
        OSLM_INFO( "MeshWriterService::configure filename: " << filename );
        m_filename = ::boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = true;
    }
}

//------------------------------------------------------------------------------

void MeshWriterService::configureWithIHM()
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
        _sDefaultPath = m_filename.parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }

}

//------------------------------------------------------------------------------

void MeshWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if(m_bServiceIsConfigured)
    {
        // Retrieve object
        ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >( );
        SLM_ASSERT("Mesh not instanced", mesh);

        ::fwDataIO::writer::MeshWriter::NewSptr writer;
        writer->setObject( mesh );
        writer->setFile(m_filename);

        try
        {
            writer->write();
        }
        catch (const std::exception & e)
        {
            std::stringstream ss;
            ss << "Warning during writing Mesh : " << e.what();

            ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Warning",
                    ss.str(),
                    ::fwGui::dialog::IMessageDialog::WARNING);
        }

    }
}

//------------------------------------------------------------------------------

}
