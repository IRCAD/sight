/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/ImageReader.hpp>

#include "devForum/tuto02/ImageReaderService.hpp"

namespace devForum
{

namespace tuto02
{

//-----------------------------------------------------------------------------

// Declare a new service of type IReader for read an Image
fwServicesRegisterMacro( ::io::IReader, ::devForum::tuto02::ImageReaderService, ::fwData::Image );

//-----------------------------------------------------------------------------

ImageReaderService::ImageReaderService() noexcept
    : m_fsImgPath("")
{
}

//-----------------------------------------------------------------------------

ImageReaderService::~ImageReaderService() noexcept
{
}

//-----------------------------------------------------------------------------

void ImageReaderService::configuring()
{
    // Test if in the service configuration the tag filename is defined. If it is defined, the image path is initialized and we tag the service as configured.
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id");
        m_fsImgPath = ::boost::filesystem::path( filename );
    }
}

//-----------------------------------------------------------------------------

void ImageReaderService::configureWithIHM()
{
}

//-----------------------------------------------------------------------------

void ImageReaderService::starting()
{
}

//-----------------------------------------------------------------------------

void ImageReaderService::stopping()
{
    SLM_TRACE_FUNC();

}

//-----------------------------------------------------------------------------

void ImageReaderService::updating()
{
    // Retrieve dataStruct associated with this service
    ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();

    // Use a reader of fwVtkIO library to read a vtk image
    ::fwVtkIO::ImageReader::sptr myReader = ::fwVtkIO::ImageReader::New();

    // Set the image (already created, but empty) that will be modifed
    myReader->setObject(associatedImage);

    // Set the file system path
    myReader->setFile(m_fsImgPath);

    // Launch reading process
    myReader->read();
}

//-----------------------------------------------------------------------------

} // end of namespace tuto02

} // end of namespace devForum
