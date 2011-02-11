/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fstream>
#include <iostream>

#include <boost/filesystem/operations.hpp>

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <fwDataIO/writer/TransformationMatrix3DWriter.hpp>

#include "ioData/TransformationMatrix3DWriterService.hpp"

namespace ioData
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::io::IWriter , ::ioData::TransformationMatrix3DWriterService , ::fwData::TransformationMatrix3D ) ;

//-----------------------------------------------------------------------------

TransformationMatrix3DWriterService::TransformationMatrix3DWriterService() :
    m_filename (""),
    m_bServiceIsConfigured(false)
{}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream << std::endl << " TransformationMatrix3D object writer" ;
}

//-----------------------------------------------------------------------------

std::vector< std::string > TransformationMatrix3DWriterService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".trf");
    return extensions ;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::starting( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

TransformationMatrix3DWriterService::~TransformationMatrix3DWriterService() throw()
{}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::configuring( ) throw(::fwTools::Failed)
{
    OSLM_TRACE( "TransformationMatrix3DWriterService::configure : " << *m_configuration );
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getValue() ;
        boost::filesystem::path location = boost::filesystem::path( filename ) ;
        m_filename = location;
        m_bServiceIsConfigured = true;
    }
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to save a transformation matrix");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TRF files","*.trf");
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

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if(m_bServiceIsConfigured)
    {
        // Retrieve object
        ::fwData::TransformationMatrix3D::sptr matrix = this->getObject< ::fwData::TransformationMatrix3D >( );
        assert( matrix ) ;

        ::fwDataIO::writer::TransformationMatrix3DWriter writer;
        writer.setObject( matrix );
        writer.setFile(m_filename);
        writer.write();

        // Notify writing
        ::fwComEd::TransformationMatrix3DMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
        ::fwServices::IEditionService::notify(this->getSptr(), this->getObject(), msg);
    }
}

//-----------------------------------------------------------------------------

}
