/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>
#include <fwComEd/TriangularMeshMsg.hpp>

#include <fwServices/ObjectMsg.hpp>

#include <io/IReader.hpp>

#include <fwCore/base.hpp>

#include <fwData/TriangularMesh.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
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

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a vtk file to load Mesh");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk","*.vtk");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

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
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Meshs ");
        myReader.addHandler( progressMeterGUI );
        myReader.read();

    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();

    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading. ";
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();

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

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        loadMesh(m_fsMeshPath, pTriangularMesh);
        notificationOfUpdate();

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void MeshReaderService::notificationOfUpdate()
{
    SLM_TRACE("MeshReaderService::notificationOfDBUpdate");
    ::fwData::TriangularMesh::sptr pTriangularMesh = this->getObject< ::fwData::TriangularMesh >();
    assert( pTriangularMesh );

    ::fwComEd::TriangularMeshMsg::NewSptr msg;;
    msg->addEvent( ::fwComEd::TriangularMeshMsg::NEW_MESH ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), pTriangularMesh, msg);
}

//------------------------------------------------------------------------------


} // namespace ioVtk
