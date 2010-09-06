/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWZIP_WXZIPFOLDER_HPP_
#define _FWZIP_WXZIPFOLDER_HPP_

#include <boost/filesystem/path.hpp>

#include "fwZip/config.hpp"

namespace fwZip
{

//------------------------------------------------------------------------------

/**
 * @brief   This interface defines static functions to zip or unzip a folder.
 * @class   ZipFolder.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 * This class provides static method to zip or unzip folder. Methods use wxWidgets API to archive data. The compression rate is equal to 0 ( simple archive ).
 */
class FWZIP_CLASS_API ZipFolder
{

public:

    /**
     * @brief This method is used to compress (zip format) a folder
     * @param[in] srcFolderName source folder path
     * @param[in] destZipFileName file path resulting to folder compression
     * @return true if folder compression is a success
     * @see this method use wxDirTraverserZIP class to realize the directory exploration
     * @todo Set in parameter the compression rate.
     */
    FWZIP_API static bool packFolder( const ::boost::filesystem::path & _srcFolderName, const ::boost::filesystem::path & _destZipFileName );

    /**
     * @brief This method is used to uncompress (zip format) a folder
     * @param[in] srcZipFileName source zip folder path
     * @param[in] destFolderName file path resulting to folder decompression
     * @return true if folder decompression is a success
     */
    FWZIP_API static bool unpackFolder( const ::boost::filesystem::path & _srcZipFileName, const ::boost::filesystem::path & _destFolderName );

};

//------------------------------------------------------------------------------

} //namespace fwZip

#endif // _FWZIP_WXZIPFOLDER_HPP_
