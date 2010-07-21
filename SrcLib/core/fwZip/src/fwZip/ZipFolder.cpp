/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "microzip/Unzipper.hpp"
#include "microzip/Zipper.hpp"
#include "fwZip/ZipFolder.hpp"

namespace fwZip
{

//------------------------------------------------------------------------------

bool ZipFolder::packFolder( const ::boost::filesystem::path & _srcFolderName, const ::boost::filesystem::path& _destZipFileName )
{
    // folder '_srcFolderName' must not be present in the archive,
    // so this folder is the root of the archive
    bool bRes = false;
    ::microzip::Zipper zip;
    if (zip.OpenZip(_destZipFileName, _srcFolderName))
    {
        bRes = zip.AddFolderToZip(_srcFolderName);
    }
    return bRes;
}

//------------------------------------------------------------------------------

bool ZipFolder::unpackFolder( const ::boost::filesystem::path & _srcZipFileName, const ::boost::filesystem::path & _destFolderName )
{
    return ::microzip::Unzipper::Unzip(_srcZipFileName, _destFolderName);
}

//------------------------------------------------------------------------------

} //namespace fwZip
