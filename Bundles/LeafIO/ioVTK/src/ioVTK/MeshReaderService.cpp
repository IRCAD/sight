/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/version.h>

#include <boost/filesystem/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>
#include <fwComEd/TriangularMeshMsg.hpp>

#include <fwServices/ObjectMsg.hpp>

#include <io/IReader.hpp>

#include <fwCore/base.hpp>

#include <fwData/TriangularMesh.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/LocationDialog.hpp>

#include <fwWX/ProgressTowx.hpp>
#include <vtkIO/MeshReader.hpp>

#include "ioVTK/MeshReaderService.hpp"


namespace ioVTK
{

REGISTER_SERVICE( ::io::IReader , ::ioVTK::MeshReaderService , ::fwData::TriangularMesh ) ;

//------------------------------------------------------------------------------

MeshReaderService::MeshReaderService() throw() :
    m_bServiceIsConfigured(false),
    m_fsMeshPath("")
{}

//------------------------------------------------------------------------------

MeshReaderService::~MeshReaderService() throw()
{}

//------------------------------------------------------------------------------

void MeshReaderService::configuring() throw(::fwTools::Failed)
{
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id") ;
        m_fsMeshPath = ::boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = ::boost::filesystem::exists(m_fsMeshPath);
        OSLM_TRACE("Filename found" << filename ) ;
    }
}

//------------------------------------------------------------------------------

void MeshReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();

    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a vtk file to load Mesh");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk","*.vtk");

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        m_fsMeshPath = result->getPath();
        m_bServiceIsConfigured = true;
        _sDefaultPath = m_fsMeshPath.branch_path();
    }
}

//------------------------------------------------------------------------------

void MeshReaderService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("MeshReaderService::starting()");
}

//------------------------------------------------------------------------------

void MeshReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("MeshReaderService::stopping()");
}

//------------------------------------------------------------------------------

void MeshReaderService::info(std::ostream &_sstream )
{
    _sstream << "MeshReaderService::info";
}

//------------------------------------------------------------------------------

void MeshReaderService::loadMesh( const ::boost::filesystem::path vtkFile, ::fwData::TriangularMesh::sptr _pTriangularMesh )
{
    SLM_TRACE("MeshReaderService::loadMesh");
    ::vtkIO::MeshReader myReader;

    myReader.setObject(_pTriangularMesh);
    myReader.setFile(vtkFile);

    try
    {
        ::fwWX::ProgressTowx progressMeterGUI("Loading Meshs ");
        myReader.addHandler( progressMeterGUI );
        myReader.read();

    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        wxString wxStmp( ss.str().c_str(), wxConvLocal );
        wxMessageBox( wxStmp, _("Warning"), wxOK|wxICON_WARNING );
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading : ";
        wxString wxStmp( ss.str().c_str(), wxConvLocal );
        wxMessageBox( wxStmp, _("Warning"), wxOK|wxICON_WARNING );
    }
}

//------------------------------------------------------------------------------

void MeshReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("MeshReaderService::updating()");

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::TriangularMesh::sptr pTriangularMesh = this->getObject< ::fwData::TriangularMesh >() ;
        assert(pTriangularMesh);

        wxBeginBusyCursor();
        loadMesh(m_fsMeshPath, pTriangularMesh);
        notificationOfUpdate();
        wxEndBusyCursor();
    }
}

//------------------------------------------------------------------------------

void MeshReaderService::notificationOfUpdate()
{
    SLM_TRACE("MeshReaderService::notificationOfDBUpdate");
    ::fwData::TriangularMesh::sptr pTriangularMesh = this->getObject< ::fwData::TriangularMesh >();
    assert( pTriangularMesh );

//  ::fwServices::IEditionService::sptr editor = ::fwServices::get< ::fwServices::IEditionService >( pTriangularMesh ) ;
//  ::fwServices::ObjectMsg::sptr msg( new ::fwServices::ObjectMsg(pTriangularMesh) ) ;
//  msg->setAllModified( ) ;
    ::fwComEd::TriangularMeshMsg::NewSptr msg;;
    msg->addEvent( ::fwComEd::TriangularMeshMsg::NEW_MESH ) ;

//  editor->notify( msg );
    ::fwServices::IEditionService::notify(this->getSptr(), pTriangularMesh, msg);
}

//------------------------------------------------------------------------------


} // namespace ioVtk
