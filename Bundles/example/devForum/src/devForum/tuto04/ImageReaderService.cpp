/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include "devForum/tuto04/ImageReaderService.hpp"

namespace devForum
{

namespace tuto04
{

//-----------------------------------------------------------------------------

// Declare a new service of type IReader for read an Image
REGISTER_SERVICE( ::io::IReader , ::devForum::tuto04::ImageReaderService , ::fwData::Image ) ;

//-----------------------------------------------------------------------------

ImageReaderService::ImageReaderService() throw()
    : ::devForum::tuto03::ImageReaderService()
{}

//-----------------------------------------------------------------------------

ImageReaderService::~ImageReaderService() throw()
{}

//-----------------------------------------------------------------------------

void ImageReaderService::configureWithIHM()
{
    m_fsImgPath="";
    SLM_TRACE_FUNC();

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a vtk file to load Mesh");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(m_fsImgPath) );
    dialogFile.addFilter("Vtk","*.vtk");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        m_fsImgPath = result->getPath();
    }
}

//------------------------------------------------------------------------------

} // end of namespace tuto04

} // end of namespace devForum
