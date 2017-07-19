/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "devForum/tuto03/ImageReaderService.hpp"

#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/ImageReader.hpp>

// Management of notifications
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>


namespace devForum
{

namespace tuto03
{

//-----------------------------------------------------------------------------

// Declare a new service of type IReader for read an Image
fwServicesRegisterMacro( ::io::IReader, ::devForum::tuto03::ImageReaderService, ::fwData::Image );

//-----------------------------------------------------------------------------

ImageReaderService::ImageReaderService() noexcept
    : ::devForum::tuto02::ImageReaderService()
{
}

//-----------------------------------------------------------------------------

ImageReaderService::~ImageReaderService() noexcept
{
}

//-----------------------------------------------------------------------------

void ImageReaderService::updating()
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

    auto sig =
        associatedImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // end of namespace tuto03

} // end of namespace devForum
