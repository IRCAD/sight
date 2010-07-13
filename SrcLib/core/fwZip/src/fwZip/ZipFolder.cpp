/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// TODO remove me
#include <wx/string.h>
#include <fwWX/wxZipFolder.hpp>
#include <fwWX/convert.hpp>
//

#include <fwCore/base.hpp>

#include "fwZip/ZipFolder.hpp"

namespace fwZip
{

//------------------------------------------------------------------------------

bool ZipFolder::packFolder( const ::boost::filesystem::path & _srcFolderName, const ::boost::filesystem::path& _destZipFileName )
{
    // TODO remove wxWidgets code and link with fwWX
    wxString srcFolderName = ::fwWX::std2wx(_srcFolderName.string());
    wxString destZipFileName = ::fwWX::std2wx(_destZipFileName.string());
    return ::fwWX::wxZipFolder::packFolder(srcFolderName, destZipFileName);
}

//------------------------------------------------------------------------------

bool ZipFolder::unpackFolder( const ::boost::filesystem::path & _srcZipFileName, const ::boost::filesystem::path & _destFolderName )
{
    // TODO remove wxWidgets code and link with fwWX
    wxString srcZipFileName = ::fwWX::std2wx(_srcZipFileName.string());
    wxString destFolderName = ::fwWX::std2wx(_destFolderName.string());
    return ::fwWX::wxZipFolder::unpackFolder(srcZipFileName, destFolderName);
}

//------------------------------------------------------------------------------

} //namespace fwZip
