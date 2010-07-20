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
    return ::microzip::Zipper::ZipFolder(_srcFolderName, _destZipFileName);
}

//------------------------------------------------------------------------------

bool ZipFolder::unpackFolder( const ::boost::filesystem::path & _srcZipFileName, const ::boost::filesystem::path & _destFolderName )
{
    //FIXME: bIgnoreFilePath temporarily set to true
    return ::microzip::Unzipper::Unzip(_srcZipFileName, _destFolderName, true);
}

//------------------------------------------------------------------------------

} //namespace fwZip
