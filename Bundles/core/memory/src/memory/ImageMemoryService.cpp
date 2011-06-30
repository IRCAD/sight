/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>
#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwMemory/IDumpService.hpp>

#include "memory/ImageMemoryService.hpp"


namespace memory
{

REGISTER_SERVICE( ::fwMemory::IMemoryService, ::memory::ImageMemoryService , ::fwData::Image );

//------------------------------------------------------------------------------

ImageMemoryService::ImageMemoryService()
{
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE );
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER );
}

//------------------------------------------------------------------------------

ImageMemoryService::~ImageMemoryService()
{
}

//------------------------------------------------------------------------------

unsigned long ImageMemoryService::getVirtualSize()
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image>();
    SLM_ASSERT("image not instanced", image);

    if ( image->getPixelType() == ::fwTools::DynamicType() )
    {
        // pixel type not specified assume empty image
        OSLM_WARN("ImageMemoryService::getVirtualSize() for image" << image << "no specified pixeltype assuming empty image");
        return 0;
    }
    else
    {
        return ::fwData::imageSizeInBytes( *image );
    }
}

//------------------------------------------------------------------------------

void ImageMemoryService::updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwComEd::ImageMsg::csptr imageMessage = ::fwComEd::ImageMsg::dynamicConstCast( _msg );
    if( imageMessage ) // If is a Image message ( != Object Message )
    {
        if ( imageMessage->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE ) || imageMessage->hasEvent( ::fwComEd::ImageMsg::BUFFER ) )
        {
            this->update();
        }
    }
}


}
