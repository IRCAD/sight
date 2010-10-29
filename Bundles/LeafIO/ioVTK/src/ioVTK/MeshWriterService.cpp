/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/TriangularMesh.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <vtkIO/MeshWriter.hpp>

#include "ioVTK/MeshWriterService.hpp"


namespace ioVTK
{

REGISTER_SERVICE( ::io::IWriter , ::ioVTK::MeshWriterService , ::fwData::TriangularMesh ) ;

//------------------------------------------------------------------------------

MeshWriterService::MeshWriterService() throw() :
    m_bServiceIsConfigured(false),
    m_fsMeshPath("")
{}

//------------------------------------------------------------------------------

MeshWriterService::~MeshWriterService() throw()
{}

//------------------------------------------------------------------------------

void MeshWriterService::configuring() throw(::fwTools::Failed)
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

void MeshWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a vtk file to save Mesh");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk","*.vtk");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

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

void MeshWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("MeshWriterService::starting()");
}

//------------------------------------------------------------------------------

void MeshWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("MeshWriterService::stopping()");
}

//------------------------------------------------------------------------------

void MeshWriterService::info(std::ostream &_sstream )
{
    _sstream << "MeshWriterService::info";
}

//------------------------------------------------------------------------------

void MeshWriterService::saveMesh( const ::boost::filesystem::path vtkFile, ::boost::shared_ptr< ::fwData::TriangularMesh > _pMesh )
{
    SLM_TRACE("MeshWriterService::saveMesh");
    ::vtkIO::MeshWriter myWriter;

    myWriter.setObject(_pMesh);
    myWriter.setFile(vtkFile);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving Meshs ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();

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
        ss << "Warning during loading : ";

        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();

    }
}

//------------------------------------------------------------------------------

void MeshWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("MeshWriterService::updating()");

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::TriangularMesh::sptr pTriangularMesh = this->getObject< ::fwData::TriangularMesh >() ;
        assert(pTriangularMesh);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        saveMesh(m_fsMeshPath,pTriangularMesh);

        cursor.setDefaultCursor();

        m_bServiceIsConfigured = false;
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
