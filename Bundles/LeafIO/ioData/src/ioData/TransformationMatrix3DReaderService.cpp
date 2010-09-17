/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fstream>
#include <iostream>
#include <boost/filesystem/operations.hpp>

#include <fwDataIO/reader/TransformationMatrix3DReader.hpp>
#include <io/IReader.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwServices/macros.hpp>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <fwCore/base.hpp>


#include "ioData/TransformationMatrix3DReaderService.hpp"

namespace ioData
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::io::IReader , ::ioData::TransformationMatrix3DReaderService , ::fwData::TransformationMatrix3D ) ;

//-----------------------------------------------------------------------------

TransformationMatrix3DReaderService::TransformationMatrix3DReaderService() :
    m_filename (""),
    m_bServiceIsConfigured(false)
{}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream << std::endl << " TransformationMatrix3D object reader" ;
}

//-----------------------------------------------------------------------------

std::vector< std::string > TransformationMatrix3DReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".trf");
    return extensions ;
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::starting( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

TransformationMatrix3DReaderService::~TransformationMatrix3DReaderService() throw()
{}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::configuring( ) throw(::fwTools::Failed)
{
    OSLM_TRACE( "TransformationMatrix3DReaderService::configure : " << *m_configuration );
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getValue() ;
        ::boost::filesystem::path location = ::boost::filesystem::path( filename ) ;
        m_filename = location;
        m_bServiceIsConfigured = true;
    }
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::configureWithIHM()
{

    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to load a transformation matrix");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("TRF files","*.trf");

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

void TransformationMatrix3DReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void TransformationMatrix3DReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if(m_bServiceIsConfigured)
    {
        // Retrieve object
        ::fwData::TransformationMatrix3D::sptr matrix = this->getObject< ::fwData::TransformationMatrix3D >( );
        assert( matrix ) ;

        ::fwDataIO::reader::TransformationMatrix3DReader reader;
        reader.setObject( matrix );
        reader.setFile(m_filename);
        reader.read();

        // Notify reading
        ::fwComEd::TransformationMatrix3DMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
        ::fwServices::IEditionService::notify(this->getSptr(), this->getObject(), msg);
    }
}

//-----------------------------------------------------------------------------

}
