/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include <fwCore/base.hpp>

#include "microzip/Unzipper.hpp"
#include "microzip/Zipper.hpp"
#include "fwZip/ZipFolder.hpp"

namespace fwZip
{

//------------------------------------------------------------------------------

ZipFolder::ZipFolder()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ZipFolder::~ZipFolder()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

bool ZipFolder::packFolder( const ::boost::filesystem::path & _srcFolderName, const ::boost::filesystem::path& _destZipFileName )
{
    double percent = 0.;
    std::string msg = "Zip folder " + _srcFolderName.string();

    unsigned int nbFiles=0;
    for(::boost::filesystem::recursive_directory_iterator it(_srcFolderName);
            it != ::boost::filesystem::recursive_directory_iterator(); ++it)
    {
        nbFiles++;
    }
    // folder '_srcFolderName' must not be present in the archive,
    // so this folder is the root of the archive
    bool bRes = false;
    int compressLevel = Z_DEFAULT_COMPRESSION;
    ::microzip::Zipper zip;
    unsigned int index=0;
    if (zip.OpenZip(_destZipFileName, _srcFolderName))
    {
        std::string filename;
        for( ::boost::filesystem::recursive_directory_iterator it(_srcFolderName);
                it != ::boost::filesystem::recursive_directory_iterator(); ++it)
        {
            percent = (float)index / (float)nbFiles;
            this->notifyProgress( percent , msg );
            if(! ::boost::filesystem::is_directory(*it))
            {
#if BOOST_FILESYSTEM_VERSION > 2
                filename = it->path().string();
#else
                filename = it->string();
#endif
                if( ::boost::ends_with(filename, ".inr.gz") ||
                        ::boost::ends_with(filename, ".vtk") )
                {
                    compressLevel = Z_NO_COMPRESSION;
                }
                else
                {
                    compressLevel = Z_DEFAULT_COMPRESSION;
                }
                bRes &= zip.AddFileToZip(*it, false, compressLevel);
            }
            index++;
        }
    }
    this->notifyProgress( 100. , msg );
    return bRes;
}

//------------------------------------------------------------------------------

bool ZipFolder::unpackFolder( const ::boost::filesystem::path & _srcZipFileName, const ::boost::filesystem::path & _destFolderName )
{
    if (!::boost::filesystem::exists(_destFolderName))
    {
        ::boost::filesystem::create_directories(_destFolderName);
    }
    ::microzip::Unzipper zip(_srcZipFileName);
    int nbFiles = zip.GetFileCount();
    if (nbFiles == 0)
        return false;

    if (! zip.GotoFirstFile())
        return false;

    double percent = 0.;
    std::string msg = "UnZip file " + _srcZipFileName.string();
    unsigned int index=0;
    do
    {
        if (! zip.UnzipFile(_destFolderName))
            return false;
        percent = (float)index / (float)nbFiles;
        this->notifyProgress( percent , msg );
        index++;
    }
    while (zip.GotoNextFile());

    return true;
}

//------------------------------------------------------------------------------

} //namespace fwZip
