/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>
#include <wx/string.h>
#include <wx/filedlg.h>
#include <wx/app.h>
#include <boost/filesystem/operations.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/macros.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/Model.hpp>
#include <fwCore/base.hpp>
#include <fwComEd/ModelMsg.hpp>
#include <fwWX/convert.hpp>

#include <vtkIO/MeshReader.hpp>
#include <fwWX/ProgressTowx.hpp>

#include "ioVTK/VtkModelReaderService.hpp"

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::io::IReader , ::ioVTK::VtkModelReaderService , ::fwData::Model );

//------------------------------------------------------------------------------

namespace ioVTK
{

VtkModelReaderService::VtkModelReaderService() throw():
    m_bServiceIsConfigured(false)
{
    SLM_INFO( "[VtkModelReaderService::VtkModelReaderService]");
    m_color                    = ::fwData::Color::NewSptr();
    m_color->setRGBA( 0.5, 0.5, 0.5, 1.0 );
}

//------------------------------------------------------------------------------

void VtkModelReaderService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream << std::endl << " vtk file reader" ;
}

//------------------------------------------------------------------------------

std::vector< std::string > VtkModelReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".vtk");
    return extensions ;
}

//------------------------------------------------------------------------------

VtkModelReaderService::~VtkModelReaderService() throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void VtkModelReaderService::configureWithIHM()
{
    static wxString _sDefaultPath = _("");
    wxString title = _("Choose a vtk file");
    wxString folder = wxFileSelector(
            title,
            _sDefaultPath,
            wxT(""),
            wxT(""),
            wxT("Vtk files (*.vtk) |*.vtk"),
#if wxCHECK_VERSION(2, 8, 0)
            wxFD_FILE_MUST_EXIST,
#else
            wxFILE_MUST_EXIST,
#endif
            wxTheApp->GetTopWindow() );

    if( folder.IsEmpty() == false)
    {
        m_fsMeshPath = ::boost::filesystem::path( ::fwWX::wx2std(folder), ::boost::filesystem::native );
        m_bServiceIsConfigured = true;
        _sDefaultPath = ::fwWX::std2wx( m_fsMeshPath.branch_path().string() );
    }
}

//------------------------------------------------------------------------------

void VtkModelReaderService::configuring( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id") ;
        OSLM_ASSERT( "vtk file doesn't exist: " << filename, ::boost::filesystem::exists(filename) );
        m_fsMeshPath = ::boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = ::boost::filesystem::exists(filename);
    }
    if( m_configuration->findConfigurationElement("color") )
    {
        ::std::string hexColor = m_configuration->findConfigurationElement("color")->getExistingAttributeValue("id") ;
        m_color->setRGBA( hexColor );
    }
}

//------------------------------------------------------------------------------

void VtkModelReaderService::updating() throw(::fwTools::Failed)
{

    if( ! m_bServiceIsConfigured )
    {
        configureWithIHM();
    }

    ::fwData::Model::sptr model = this->getObject< ::fwData::Model >( );
    assert( model ) ;

    ::fwData::Model::NewSptr backupModel;
    backupModel->shallowCopy(model);

    model->getRefMap().clear();

    /// Create a empty triangularMesh
    ::fwData::TriangularMesh::NewSptr mesh;
    loadMesh( m_fsMeshPath, mesh );

    ::fwData::Material::NewSptr dataMat;
    dataMat->ambient()->setCRefRGBA(m_color->getCRefRGBA());
    model->getRefMap().insert (    std::pair< ::fwData::TriangularMesh::sptr , ::fwData::Material::sptr >(mesh, dataMat));

    ::fwComEd::ModelMsg::NewSptr msg;;
    msg->addEvent( ::fwComEd::ModelMsg::NEW_MODEL, backupModel ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), model, msg);
}

//------------------------------------------------------------------------------

void VtkModelReaderService::loadMesh( const ::boost::filesystem::path vtkFile, ::fwData::TriangularMesh::sptr _pTriangularMesh )
{
    SLM_TRACE_FUNC();
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


}

