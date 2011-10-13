/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

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
    wxString title = _("Choose an vtk file to load an image"); // use _("...") for support string internationalization

    // Create a dialog box to choose a file of type .vtk
    wxString file = wxFileSelector(
            title,
            wxT(""),
            wxT(""),
            wxT(""),
            wxT("Vtk (*.vtk)|*.vtk"),
            wxFD_OPEN,
            wxTheApp->GetTopWindow() );

    // If the user choose an vtk file, the image path is initialized
    if( file.IsEmpty() == false )
    {
        m_fsImgPath = ::boost::filesystem::path( wxConvertWX2MB(file) );
    }
}

//------------------------------------------------------------------------------

} // end of namespace tuto04

} // end of namespace devForum
