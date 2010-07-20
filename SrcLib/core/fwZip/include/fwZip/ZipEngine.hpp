/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWZIP_ZIPENGINE_HPP_
#define _FWZIP_ZIPENGINE_HPP_

#include <zip.h>
#include <boost/shared_ptr.hpp>

#include "fwZip/config.hpp"
#include "fwZip/reader/IObjectReader.hpp"
#include "fwZip/config.hpp"



namespace fwZip
{


/**
 * @brief   a compressor to pack/unpack Zip archive
 * @class   GenericObjectReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2010
 */

class  FWZIP_CLASS_API ZipEngine
{

public :

    FWZIP_API ZipEngine();


    /**
     * @brief compress data inside srcFolder in the zip archive dstZipArchive.
     * @note if dstZipArchive already exists then it will be overwritten
     */

    FWZIP_API void packFolder( ::boost::filesystem::path srcFolder,
                     ::boost::filesystem::path dstZipArchive
                   );



protected :

    void processFolder( ::boost::filesystem::path folder, bool skip );
    void processFile( ::boost::filesystem::path file );

    // return the relative path of the abspath relative to m_root path
    const char * stripPath( ::boost::filesystem::path abspath );

    //char m_defaultLeveLCompression; //0-9
    //std::map< std::string, char > m_specificLevelCompression;

    struct zip *m_zip;
    ::boost::filesystem::path m_root; // the root folder (=srcFolder)

};


} // namespace fwZip


#endif // _FWZIP_ZIPENGINE_HPP_
