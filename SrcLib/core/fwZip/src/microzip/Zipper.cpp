#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>

#ifndef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/conversion.hpp>

#include "microzip/Zipper.hpp"

namespace microzip
{

const unsigned int BUFFERSIZE = 2048;

//------------------------------------------------------------------------------

Zipper::Zipper() : m_uzFile(0)
{
    this->CloseZip();
}

//------------------------------------------------------------------------------

Zipper::Zipper(::boost::filesystem::path filePath, ::boost::filesystem::path rootFolder, bool bAppend) : m_uzFile(0)
{
    this->CloseZip();
    this->OpenZip(filePath, rootFolder, bAppend);
}

//------------------------------------------------------------------------------

Zipper::~Zipper()
{
    this->CloseZip();
}

//------------------------------------------------------------------------------

bool Zipper::CloseZip()
{
    int nRet = m_uzFile ? zipClose(m_uzFile, NULL) : ZIP_OK;

    m_uzFile = NULL;
    m_rootFolder = "";
    return (nRet == ZIP_OK);
}

//------------------------------------------------------------------------------

void Zipper::GetFileInfo(ZipFileInfo& info)
{
    info = m_info;
}

//------------------------------------------------------------------------------

bool Zipper::ZipFile(::boost::filesystem::path filePath, ::boost::filesystem::path zipFilePath, int level)
{
    bool res = false;
    // make zip path
    if(zipFilePath.empty())
    {
       zipFilePath = ::boost::filesystem::change_extension(filePath, ".zip");
    }

    Zipper zip;
    if (zip.OpenZip(zipFilePath, filePath.parent_path(), false))
    {
        res = zip.AddFileToZip(filePath, false, level);
    }
    return res;
}

//------------------------------------------------------------------------------

bool Zipper::ZipFolder(::boost::filesystem::path folderPath, ::boost::filesystem::path zipFilePath, bool bIgnoreFilePath, int level)
{
    bool res = false;
    // make zip path
    if(zipFilePath.empty())
    {
        ::boost::filesystem::path zipFile(folderPath.filename() + ".zip");
        zipFilePath = folderPath.parent_path() / zipFile ;
    }
    Zipper zip;
    if (zip.OpenZip(zipFilePath, folderPath.parent_path(), false))
    {
        res = zip.AddFolderToZip(folderPath, bIgnoreFilePath, level);
    }
    return res;
}

//------------------------------------------------------------------------------

bool Zipper::AddFileToZip(::boost::filesystem::path filePath, bool bIgnoreFilePath, int level)
{
    if (!m_uzFile)
        return false;

    ::boost::filesystem::path fileName;
    if (bIgnoreFilePath)
    {
        fileName = filePath.filename();
    }
    else
    {
        fileName = this->relative_path(filePath, m_rootFolder);
    }

    // save file attributes
    zip_fileinfo zfi;
    zfi.internal_fa = 0;
    zfi.external_fa = this->getFileAttributes(filePath);

    // save file time
    std::time_t lastWriteTime = ::boost::filesystem::last_write_time( filePath ) ;
    ::boost::posix_time::ptime pt = ::boost::posix_time::from_time_t(lastWriteTime);
    std::tm ptm = ::boost::posix_time::to_tm(pt);
    zfi.dosDate = 0;
    zfi.tmz_date.tm_year = ptm.tm_year;
    zfi.tmz_date.tm_mon  = ptm.tm_mon;
    zfi.tmz_date.tm_mday = ptm.tm_mday;
    zfi.tmz_date.tm_hour = ptm.tm_hour;
    zfi.tmz_date.tm_min  = ptm.tm_min;
    zfi.tmz_date.tm_sec  = ptm.tm_sec;

    // load input file
    std::ifstream inputFile;
    inputFile.open(filePath.string().c_str(), std::ios::in | std::ios::binary);
    if (!inputFile.is_open())
    {
        return false;
    }

    int nRet = zipOpenNewFileInZip(m_uzFile,
            fileName.string().c_str(),
            &zfi,
            NULL,
            0,
            NULL,
            0,
            NULL,
            Z_DEFLATED,
            level);

    if (nRet == ZIP_OK)
    {
        m_info.nFileCount++;

        // read the file and output to zip
        char pBuffer[BUFFERSIZE];
        unsigned long dwBytesRead = 0, dwFileSize = 0;

        while (nRet == ZIP_OK && inputFile.good())
        {
            inputFile.read(pBuffer, BUFFERSIZE);
            dwBytesRead = inputFile.gcount();
            dwFileSize += dwBytesRead;

            if (dwBytesRead)
                nRet = zipWriteInFileInZip(m_uzFile, pBuffer, dwBytesRead);
            else
                break;
        }

        m_info.dwUncompressedSize += dwFileSize;
    }

    zipCloseFileInZip(m_uzFile);
    inputFile.close();

    return (nRet == ZIP_OK);
}

//------------------------------------------------------------------------------

bool Zipper::AddFileToZip(::boost::filesystem::path filePath, ::boost::filesystem::path relFolderPath, int level)
{
    if (!m_uzFile)
        return false;

    if (relFolderPath.empty())
        return false;

    // save file attributes and time
    zip_fileinfo zfi;
    zfi.internal_fa = 0;
    zfi.external_fa = this->getFileAttributes(filePath);

    // save file time
    std::time_t lastWriteTime = ::boost::filesystem::last_write_time( filePath ) ;
    ::boost::posix_time::ptime pt = ::boost::posix_time::from_time_t(lastWriteTime);
    std::tm ptm = ::boost::posix_time::to_tm(pt);
    zfi.dosDate = 0;
    zfi.tmz_date.tm_year = ptm.tm_year;
    zfi.tmz_date.tm_mon  = ptm.tm_mon;
    zfi.tmz_date.tm_mday = ptm.tm_mday;
    zfi.tmz_date.tm_hour = ptm.tm_hour;
    zfi.tmz_date.tm_min  = ptm.tm_min;
    zfi.tmz_date.tm_sec  = ptm.tm_sec;

    // load input file
    std::ifstream inputFile;
    inputFile.open(filePath.string().c_str(), std::ios::in | std::ios::binary);
    if (!inputFile.is_open())
    {
        return false;
    }

    ::boost::filesystem::path fileName(filePath.filename());
    ::boost::filesystem::path zipFilePath = relFolderPath.relative_path() / fileName;

    // open the file in the zip making sure we remove any leading '\'
    int nRet = zipOpenNewFileInZip(m_uzFile,
            zipFilePath.string().c_str(),
            &zfi,
            NULL,
            0,
            NULL,
            0,
            NULL,
            Z_DEFLATED,
            level);

    if (nRet == ZIP_OK)
    {
        m_info.nFileCount++;

        // read the file and output to zip
        char pBuffer[BUFFERSIZE];
        unsigned long dwBytesRead = 0, dwFileSize = 0;

        while (nRet == ZIP_OK && inputFile.good() )
        {
            inputFile.read(pBuffer, BUFFERSIZE);
            dwBytesRead = inputFile.gcount();
            dwFileSize += dwBytesRead;

            if (dwBytesRead)
                nRet = zipWriteInFileInZip(m_uzFile, pBuffer, dwBytesRead);
            else
                break;
        }

        m_info.dwUncompressedSize += dwFileSize;
    }

    zipCloseFileInZip(m_uzFile);
    inputFile.close();

    return (nRet == ZIP_OK);
}

//------------------------------------------------------------------------------

bool Zipper::AddFolderToZip(::boost::filesystem::path folderPath, bool bIgnoreFilePath, int level)
{
    if (!m_uzFile)
        return false;

    m_info.nFolderCount++;

    // save file attributes
    zip_fileinfo zfi;
    zfi.internal_fa = 0;
    zfi.external_fa = this->getFileAttributes(folderPath);

    std::time_t lastWriteTime = ::boost::filesystem::last_write_time( folderPath ) ;
    ::boost::posix_time::ptime pt = ::boost::posix_time::from_time_t(lastWriteTime);
    std::tm ptm = ::boost::posix_time::to_tm(pt);
    zfi.dosDate = 0;
    zfi.tmz_date.tm_year = ptm.tm_year;
    zfi.tmz_date.tm_mon  = ptm.tm_mon;
    zfi.tmz_date.tm_mday = ptm.tm_mday;
    zfi.tmz_date.tm_hour = ptm.tm_hour;
    zfi.tmz_date.tm_min  = ptm.tm_min;
    zfi.tmz_date.tm_sec  = ptm.tm_sec;

    ::boost::filesystem::path folderName;
    if (bIgnoreFilePath)
    {
        folderName = ::boost::filesystem::path(folderPath.filename());
    }
    else
    {
        folderName = this->relative_path(folderPath, m_rootFolder);
    }

    // open the file in the zip
    int nRet = zipOpenNewFileInZip(m_uzFile,
            (folderName.string()+"/").c_str(),
            &zfi,
            NULL,
            0,
            NULL,
            0,
            NULL,
            Z_DEFLATED,
            level);

    zipCloseFileInZip(m_uzFile);

    ::boost::filesystem::directory_iterator iter(folderPath);
    ::boost::filesystem::directory_iterator end;
    for ( iter ; iter!=end ; ++iter)
    {
        if ( ::boost::filesystem::is_regular_file(iter->status()) )
        {
            this->AddFileToZip( iter->path(), bIgnoreFilePath, level );
        }
        else if ( ::boost::filesystem::is_directory(iter->status()) )
        {
            this->AddFolderToZip( iter->path(), bIgnoreFilePath, level );
        }
    }
    return true;
}

//------------------------------------------------------------------------------

bool Zipper::OpenZip(::boost::filesystem::path filePath, ::boost::filesystem::path rootFolder, bool bAppend)
{
    this->CloseZip();

    if (filePath.empty())
        return false;

    int append = APPEND_STATUS_CREATE;
    /*
     * if the file pathname exist and append==APPEND_STATUS_CREATEAFTER,
     * the zip will be created at the end of the file.
     * (useful if the file contain a self extractor code)
     * if the file pathname exist and append==APPEND_STATUS_ADDINZIP, we will
     * add files in existing zip (be sure you don't add file that doesn't exist)
     */
    if (::boost::filesystem::exists(filePath))
    {
        append = (bAppend ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE);
    }
    m_uzFile = zipOpen(filePath.string().c_str(), append);

    if (m_uzFile)
    {
        if (rootFolder.empty())
        {
            m_rootFolder = filePath.parent_path();
        }
        else
        {
            m_rootFolder = rootFolder;
        }
    }
    return (m_uzFile != NULL);
}

//------------------------------------------------------------------------------

::boost::filesystem::path Zipper::relative_path( ::boost::filesystem::path const path, ::boost::filesystem::path const base)
{
    // see https://svn.boost.org/trac/boost/ticket/1976
    if (path.has_root_path())
    {
        if (path.root_path() != base.root_path())
        {
            return path;
        }
        else
        {
            return this->relative_path(path.relative_path(), base.relative_path());
        }
    }
    else
    {
        if (base.has_root_path())
        {
            throw "cannot uncomplete a path relative path from a rooted base";
        }
        else
        {
            typedef ::boost::filesystem::path::const_iterator path_iterator;
            path_iterator path_it = path.begin();
            path_iterator base_it = base.begin();
            while ( path_it != path.end() && base_it != base.end() )
            {
                if (*path_it != *base_it) break;
                ++path_it;
                ++base_it;
            }
            ::boost::filesystem::path result;
            for (; base_it != base.end(); ++base_it)
            {
                result /= "..";
            }
            for (; path_it != path.end(); ++path_it)
            {
                result /= *path_it;
            }
            return result;
        }
    }
}
//------------------------------------------------------------------------------

unsigned long Zipper::getFileAttributes(::boost::filesystem::path const filePath)
{
    unsigned long attrib = 0;
#ifdef _WIN32
    attrib = ::GetFileAttributes(filePath.native_directory_string().c_str());
#else
    struct stat path_stat;
    if ( ::stat( filePath.native_directory_string().c_str(), &path_stat ) == 0 )
    {
        attrib = path_stat.st_mode;
    }
#endif
    return attrib;
}

//------------------------------------------------------------------------------

} // namespace microzip

