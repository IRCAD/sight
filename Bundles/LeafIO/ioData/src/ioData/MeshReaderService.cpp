/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/filesystem/operations.hpp>

#include <io/IReader.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwCore/base.hpp>
#include <fwServices/macros.hpp>

#include <fwDataIO/reader/MeshReader.hpp>

#include <fwComEd/MeshMsg.hpp>

#include "ioData/MeshReaderService.hpp"

REGISTER_SERVICE( ::io::IReader , ::ioData::MeshReaderService , ::fwData::Mesh ) ;

namespace ioData
{

MeshReaderService::MeshReaderService():
        m_fsMeshPath (""),
        m_bServiceIsConfigured(false)
{
}

//-----------------------------------------------------------------------------

void MeshReaderService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream << std::endl << "Trian file reader" ;
}

//-----------------------------------------------------------------------------

std::vector< std::string > MeshReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".trian");
    return extensions ;
}

//-----------------------------------------------------------------------------

MeshReaderService::~MeshReaderService() throw()
{
}

//------------------------------------------------------------------------------

void MeshReaderService::configuring( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getValue() ;
        OSLM_INFO( "MeshReaderService::configure filename: " << filename );
        m_fsMeshPath = ::boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = true;
    }
}

//------------------------------------------------------------------------------

void MeshReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a trian file");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Trian file","*.trian");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        m_fsMeshPath = result->getPath();
        m_bServiceIsConfigured = true;
        _sDefaultPath = m_fsMeshPath.parent_path().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
}

//------------------------------------------------------------------------------

void MeshReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if(m_bServiceIsConfigured)
    {
        // Retrieve object
        ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >( );
        SLM_ASSERT("mesh not instanced", mesh);

        ::fwDataIO::reader::MeshReader reader;
        reader.setObject( mesh );
        reader.setFile(m_fsMeshPath);
        reader.read();

        // Notify reading
        ::fwComEd::MeshMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::MeshMsg::NEW_MESH );
        ::fwServices::IEditionService::notify(this->getSptr(), mesh, msg);
    }
}

}
