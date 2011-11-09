/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

#include <vtkIO/ImageReader.hpp>

// Management of Image Message
#include <fwComEd/ImageMsg.hpp>

// Management of notifications
#include <fwServices/IEditionService.hpp>

#include "devForum/tuto03/ImageReaderService.hpp"

namespace devForum
{

namespace tuto03
{

//-----------------------------------------------------------------------------

// Declare a new service of type IReader for read an Image
REGISTER_SERVICE( ::io::IReader , ::devForum::tuto03::ImageReaderService , ::fwData::Image ) ;

//-----------------------------------------------------------------------------

ImageReaderService::ImageReaderService() throw()
    : ::devForum::tuto02::ImageReaderService()
{}

//-----------------------------------------------------------------------------

ImageReaderService::~ImageReaderService() throw()
{}

//-----------------------------------------------------------------------------

void ImageReaderService::updating() throw ( ::fwTools::Failed )
{
    // First, called updating of ::devForum::tuto02::ImageReaderService::updating to
    // process reading
    ::devForum::tuto02::ImageReaderService::updating();

    // Then, notifies listerners that the image has been modified
    notifyMessage();
}

//-----------------------------------------------------------------------------

void ImageReaderService::notifyMessage()
{
    ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();
    assert( associatedImage );

    // Creation of an image message to say that image is an new image
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::NEW_IMAGE ) ;

    // Notifies message to all service listeners
    ::fwServices::IEditionService::notify( this->getSptr(), associatedImage, msg );
}

//------------------------------------------------------------------------------

} // end of namespace tuto03

} // end of namespace devForum
