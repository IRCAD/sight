/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>


#include <zlib.h>

#include <fwTools/pathDifference.hpp>

#include <fwData/Object.hpp>
#include <fwData/Image.hpp>

#include "fwZip/ZipEngine.hpp"


namespace fwZip
{


ZipEngine::ZipEngine()
:m_zip(0)
 {}



std::string ZipEngine::stripPath( ::boost::filesystem::path path )
{
    return ::fwTools::getPathDifference(m_root,path).string();
}



void ZipEngine::processFolder( ::boost::filesystem::path folder, bool skip )
{
    namespace fs = ::boost::filesystem;
    SLM_ASSERT( "must be a folder", fs::is_directory(folder) );

    if ( !skip )
    {
        int index = 0;
        index = zip_add_dir( m_zip, stripPath( folder ).c_str() );
        SLM_ASSERT( "error adding in zip archive the folder " << folder.string(), index>=0 );
    }

    fs::path tt;
    fs::directory_iterator iter(folder);
    fs::directory_iterator end; //// default construction yields past-the-end
    for ( iter ; iter!=end ; ++iter)
    {
        std::cout << iter->path().filename() << ' '; // display filename only
        if ( is_regular_file(iter->status()) )
        {
            processFile( iter->path() );
        }
        else if ( fs::is_directory(iter->status()) )
        {
                    processFolder( iter->path(), false );
        }
        std::cout  << '\n';

     }

}


void ZipEngine::processFile( ::boost::filesystem::path file )
{
    int err;
    zip_source *source = zip_source_file( m_zip, file.string().c_str(), 0, 0);
    err = zip_add(m_zip, stripPath(file).c_str() , source );
    SLM_ASSERT( "error adding in zip archive the file " << file.string(), err!=-1 );
}




void ZipEngine::packFolder( ::boost::filesystem::path srcFolder,
                 ::boost::filesystem::path dstZipArchive
               )
{
    namespace fs = ::boost::filesystem;
    m_root = srcFolder;

    if ( fs::exists(dstZipArchive) )
    {
        fs::remove(dstZipArchive);
    }



    int errCode=0;
    m_zip = zip_open( dstZipArchive.string().c_str() ,  ZIP_CREATE, &errCode );
    SLM_ASSERT("Error creating zip archive" << dstZipArchive.string(), m_zip );

    processFolder(srcFolder,true);

    errCode= zip_close(m_zip);
    SLM_ASSERT("Error closing zip archive" << dstZipArchive.string(), errCode == 0 );



}



} // namespace fwZip
