/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include "devForumQt/tuto04/ImageReaderService.hpp"
#include "devForumQt/tuto04/ImageReaderActionService.hpp"

namespace devForum
{

namespace tuto04
{

//-----------------------------------------------------------------------------

// Declare a new service of type IReader for read an Image
REGISTER_SERVICE( ::fwGui::IActionSrv , ::devForum::tuto04::ImageReaderActionService , ::fwData::Image ) ;

//-----------------------------------------------------------------------------

ImageReaderActionService::ImageReaderActionService() throw()
{
}
//-----------------------------------------------------------------------------

ImageReaderActionService::~ImageReaderActionService() throw()
{}

//-----------------------------------------------------------------------------

void ImageReaderActionService::configuring() throw ( ::fwTools::Failed )
{
    // Uses parent method to parse the service configuration and retrieves
    // the name of action in Menu, his shortcut, and some others parameters that
    // permits to create the item menu in IHM.
    this->initialize();
}

//-----------------------------------------------------------------------------

void ImageReaderActionService::starting() throw ( ::fwTools::Failed )
{
    // Uses parent method to create item menu and append it in the application menu
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void ImageReaderActionService::stopping() throw ( ::fwTools::Failed )
{
    // Uses parent method to remove item menu of the application
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void ImageReaderActionService::updating() throw ( ::fwTools::Failed )
{
    // Get object associated to this service
    ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();

    // Create a new service to read vtk image
    ImageReaderService::sptr readerSrv ( new ImageReaderService() );

    // Register this new service in Object Service Registry of the current application
    ::fwServices::OSR::registerService( associatedImage, readerSrv );

    // Start the service ( does nothing )
    readerSrv->start();
    // configure the service ( open a dialog box to choose vtk image file on filesystem )
    readerSrv->configureWithIHM();
    // Update service ( read file and notify listeners that we have new image laoded )
    readerSrv->update();
    // Stop service before killing it ( dos nothing )
    readerSrv->stop();

    // Before destruction of the reader service ( end of this method because it
    // is a shared ptr ), we unregister it of the Object Service Registry
    ::fwServices::OSR::unregisterService( readerSrv );
}

//-----------------------------------------------------------------------------

void ImageReaderActionService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

} // end of namespace tuto04

} // end of namespace devForum
