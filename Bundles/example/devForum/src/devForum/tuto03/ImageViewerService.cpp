/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// Image object
#include <fwData/Image.hpp>

// Services tools
#include <fwServices/macros.hpp>

#include "devForum/tuto03/ImageViewerService.hpp"

namespace devForum
{

namespace tuto03
{

//-----------------------------------------------------------------------------

// Declare a new service of type IRender for render Image
fwServicesRegisterMacro( ::fwRender::IRender, ::devForum::tuto03::ImageViewerService, ::fwData::Image );

//-----------------------------------------------------------------------------

ImageViewerService::ImageViewerService() throw()
    : ::devForum::tuto02::ImageViewerService()
{
}

//-----------------------------------------------------------------------------

ImageViewerService::~ImageViewerService() throw()
{
}

//-----------------------------------------------------------------------------
void ImageViewerService::configuring() throw ( ::fwTools::Failed )
{
    ::devForum::tuto02::ImageViewerService::configuring();
}
//-----------------------------------------------------------------------------

void ImageViewerService::stopping() throw ( ::fwTools::Failed )
{
    ::devForum::tuto02::ImageViewerService::stopping();
}
//-----------------------------------------------------------------------------

} // end of namespace tuto03

} // end of namespace devForum
