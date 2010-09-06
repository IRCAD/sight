/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwCore/base.hpp>

#include <fwTools/Failed.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/macros.hpp>

#include <fwDataIO/writer/GzBufferImageWriter.hpp>
#include <fwDataIO/reader/GzBufferImageReader.hpp>

#include "fwXML/ImageFileFormatService.hpp"

REGISTER_SERVICE( ::fwXML::IFileFormatService , fwXML::ImageFileFormatService , ::fwData::Image);

namespace fwXML
{

std::string ImageFileFormatService::m_preferedWriter;

//------------------------------------------------------------------------------

ImageFileFormatService::ImageFileFormatService()
{
    RWPoliciesInstall();
}

//------------------------------------------------------------------------------

ImageFileFormatService::~ImageFileFormatService()
{}

//------------------------------------------------------------------------------

void ImageFileFormatService::setPreferedWriter( std::string libwriter )
{
    m_preferedWriter = libwriter;
}

//------------------------------------------------------------------------------

void ImageFileFormatService::RWPoliciesInstall()
{
    // try to install inrReader
    ::fwDataIO::reader::IObjectReader::sptr reader;

    reader = fwTools::ClassFactoryRegistry::create< ::fwDataIO::reader::IObjectReader, std::string >("::vtkIO::ImageReader" );
    if(reader)
    {
        setReader(reader);
    }
    else
    {
        reader = fwTools::ClassFactoryRegistry::create< ::fwDataIO::reader::IObjectReader, std::string >( "::itkIO::ImageReader" );
        if ( reader )
        {
            setReader( reader );
        }
        else
        {
            setReader( ::boost::shared_ptr< ::fwDataIO::reader::GzBufferImageReader >( new ::fwDataIO::reader::GzBufferImageReader() ));
        }
    }

    // try to install Writrer
    ::fwDataIO::writer::IObjectWriter::sptr writer;
    writer = fwTools::ClassFactoryRegistry::create< ::fwDataIO::writer::IObjectWriter, std::string >(m_preferedWriter);
    OSLM_WARN_IF(" prefered Writer" << m_preferedWriter << " cannot be instanciated => use default one", !m_preferedWriter.empty() && !writer);
    if ( writer )
    {
        setWriter( writer );
    }
    else
    {
        writer = fwTools::ClassFactoryRegistry::create< ::fwDataIO::writer::IObjectWriter, std::string >( "::vtkIO::ImageWriter" );
        if ( writer )
        {
            setWriter( writer );
        }
        else
        {
            writer = fwTools::ClassFactoryRegistry::create< ::fwDataIO::writer::IObjectWriter, std::string >( "::itkIO::ImageWriter" );
            if ( writer )
            {
                setWriter( writer );
            }
            else
            {
                setWriter( ::boost::shared_ptr< ::fwDataIO::writer::GzBufferImageWriter >( new ::fwDataIO::writer::GzBufferImageWriter() ));
            }

        }
    }

}

//------------------------------------------------------------------------------

void ImageFileFormatService::load()
{
    assert( !m_filename.empty() );
    // assert( !m_localFolder.empty() ); not mandatory can be loaded at root Folder

    //RWPoliciesInstall();
    assert( m_reader );

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >() ;
    assert( image ) ;

    m_reader->setObject(image);
    ::fwData::location::SingleFile::sptr path( new ::fwData::location::SingleFile() );
    path->setPath( this->getFullPath() );
    m_reader->setLocation( path );
    m_reader->read();
}

//------------------------------------------------------------------------------

void ImageFileFormatService::save()
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Filename is empty", !m_filename.empty() );

    RWPoliciesInstall();
    assert( m_writer );

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >() ;
    assert( image ) ;

    if (image->getBuffer() )
    {
        m_writer->setObject(image);
        this->extension() = m_writer->extension();
        ::fwData::location::SingleFile::sptr path( new ::fwData::location::SingleFile() );
        path->setPath( this->getFullPath() );
        m_writer->setLocation( path );
        m_writer->write();
    }
}

//------------------------------------------------------------------------------

}
