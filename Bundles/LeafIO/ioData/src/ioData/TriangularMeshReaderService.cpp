/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>
#include <wx/wx.h>

#include <boost/filesystem/operations.hpp>

#include <io/IReader.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwCore/base.hpp>
#include <fwServices/macros.hpp>

#include <fwDataIO/reader/TriangularMeshReader.hpp>

#include <fwComEd/TriangularMeshMsg.hpp>

#include "ioData/TriangularMeshReaderService.hpp"

REGISTER_SERVICE( ::io::IReader , ::ioData::TriangularMeshReaderService , ::fwData::TriangularMesh ) ;

namespace ioData
{

TriangularMeshReaderService::TriangularMeshReaderService()
{
}

//-----------------------------------------------------------------------------

void TriangularMeshReaderService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream << std::endl << " Triangular Mesh reader" ;
}

//-----------------------------------------------------------------------------

std::vector< std::string > TriangularMeshReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".trian");
    return extensions ;
}

//-----------------------------------------------------------------------------

TriangularMeshReaderService::~TriangularMeshReaderService() throw()
{
}

//------------------------------------------------------------------------------

void TriangularMeshReaderService::configuring( ) throw(::fwTools::Failed)
{
    OSLM_INFO( "TriangularMeshReaderService::configure : " << *m_configuration );
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getValue() ;
        OSLM_INFO( "TriangularMeshReaderService::configure filename: " << filename );
        m_fsMeshPath = boost::filesystem::path( filename ) ;
    }
}

//------------------------------------------------------------------------------

void TriangularMeshReaderService::configureWithIHM()
{
    static wxString _sDefaultPath = _("");
    wxString title = _("Choose an TrianMesh file");
    wxString folder = wxFileSelector(
            title,
            _sDefaultPath,
            wxT(""),
            wxT(""),
            wxT("TrianMesh (*.trian)|*.trian"),
            wxFD_FILE_MUST_EXIST,
            wxTheApp->GetTopWindow() );

    if( folder.IsEmpty() == false)
    {
        m_fsMeshPath = ::boost::filesystem::path( wxConvertWX2MB(folder), ::boost::filesystem::native );
        _sDefaultPath = wxConvertMB2WX( m_fsMeshPath.branch_path().string().c_str() );
    }
}

//------------------------------------------------------------------------------

void TriangularMeshReaderService::updating() throw(::fwTools::Failed)
{
    SLM_INFO("[TriangularMeshReaderService::update]");

    if (::boost::filesystem::exists(m_fsMeshPath))
    {
        // Retrieve object
        ::fwData::TriangularMesh::sptr mesh = this->getObject< ::fwData::TriangularMesh >( );
        assert( mesh ) ;

        ::fwDataIO::reader::TriangularMeshReader reader;
        reader.setObject( mesh );
        reader.setFile(m_fsMeshPath);
        reader.read();

        // Notify reading
        ::fwComEd::TriangularMeshMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::TriangularMeshMsg::NEW_MESH );
        ::fwServices::IEditionService::notify(this->getSptr(), mesh, msg);
    }
}
}
