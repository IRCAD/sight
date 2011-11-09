/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// Image object
#include <fwData/Image.hpp>

// Services tools
#include <fwServices/Base.hpp>

// Management of Image Message
#include <fwComEd/ImageMsg.hpp>

#include "devForumQt/tuto03/ImageViewerService.hpp"

namespace devForum
{

namespace tuto03
{

//-----------------------------------------------------------------------------

// Declare a new service of type IRender for render Image
REGISTER_SERVICE( ::fwRender::IRender , ::devForum::tuto03::ImageViewerService , ::fwData::Image ) ;

//-----------------------------------------------------------------------------

ImageViewerService::ImageViewerService() throw()
    : ::devForum::tuto02::ImageViewerService()
{
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE );
}

//-----------------------------------------------------------------------------

ImageViewerService::~ImageViewerService() throw()
{}

//-----------------------------------------------------------------------------
void ImageViewerService::configuring() throw ( ::fwTools::Failed )
{
    ::devForum::tuto02::ImageViewerService::configuring();
}
//-----------------------------------------------------------------------------

void ImageViewerService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    // If message is a ImageMsg and if event is NEW IMAGE
    ::fwComEd::ImageMsg::csptr pImageMsg = ::fwComEd::ImageMsg::dynamicConstCast( _msg ) ;
    if ( pImageMsg && pImageMsg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE ) )
    {
        updateVTKPipeline();
    }
}

//-----------------------------------------------------------------------------
void ImageViewerService::stopping() throw ( ::fwTools::Failed )
{
    ::devForum::tuto02::ImageViewerService::stopping();
}
//-----------------------------------------------------------------------------

} // end of namespace tuto03

} // end of namespace devForum
