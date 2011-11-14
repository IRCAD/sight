/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/filesystem/operations.hpp>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/macros.hpp>

#include <fwData/TriangularMesh.hpp>
#include <fwData/Model.hpp>
#include <fwData/location/Folder.hpp>

#include <fwComEd/ModelMsg.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <vtkIO/TriangularMeshReader.hpp>

#include "ioVTK/VtkModelReaderService.hpp"

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::io::IReader , ::ioVTK::VtkModelReaderService , ::fwData::Model );

//------------------------------------------------------------------------------

namespace ioVTK
{

VtkModelReaderService::VtkModelReaderService() throw():
    m_bServiceIsConfigured(false)
{
    SLM_TRACE_FUNC();
    m_color = ::fwData::Color::NewSptr();
    m_color->setRGBA( 0.5, 0.5, 0.5, 1.0 );
}

//------------------------------------------------------------------------------

void VtkModelReaderService::info(std::ostream &_sstream )
{
    this->BaseClass::info( _sstream ) ;
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
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose an vtk file to load a model");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk","*.vtk");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath();
        m_fsMeshPath = result->getPath();
        m_bServiceIsConfigured = true;
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
    SLM_ASSERT("model not instanced", model);

    ::fwData::Model::NewSptr backupModel;
    backupModel->shallowCopy(model);

    model->getRefMap().clear();

    /// Create a empty triangularMesh
    ::fwData::TriangularMesh::NewSptr mesh;
    this->loadMesh( m_fsMeshPath, mesh );

    ::fwData::Material::NewSptr dataMat;
    dataMat->ambient()->setCRefRGBA(m_color->getCRefRGBA());
    model->getRefMap()[ mesh ] = dataMat ;

    ::fwComEd::ModelMsg::NewSptr msg;;
    msg->addEvent( ::fwComEd::ModelMsg::NEW_MODEL, backupModel ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), model, msg);
}

//------------------------------------------------------------------------------

void VtkModelReaderService::loadMesh( const ::boost::filesystem::path vtkFile, ::fwData::TriangularMesh::sptr _pTriangularMesh )
{
    SLM_TRACE_FUNC();

    ::vtkIO::TriangularMeshReader::NewSptr myReader;
    myReader->setObject(_pTriangularMesh);
    myReader->setFile(vtkFile);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Mesh");
        myReader->addHandler( progressMeterGUI );
        myReader->read();
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
        ss << "Warning during loading.";

        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------


}

