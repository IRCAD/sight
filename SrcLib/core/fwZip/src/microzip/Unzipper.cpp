#include <cstring>
#include <iostream>
#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include "microzip/Unzipper.hpp"

#ifndef _WIN32
#include <sys/stat.h>
#endif

namespace microzip
{

const unsigned int BUFFERSIZE = 2048;

//------------------------------------------------------------------------------

Unzipper::Unzipper() : m_uzFile(0)
{}

//------------------------------------------------------------------------------

Unzipper::Unzipper(::boost::filesystem::path filePath) : m_uzFile(0)
{
    this->OpenZip(filePath);
}

//------------------------------------------------------------------------------

Unzipper::~Unzipper()
{
    this->CloseZip();
}

//------------------------------------------------------------------------------

bool Unzipper::CloseZip()
{
    unzCloseCurrentFile(m_uzFile);

    int nRet = unzClose(m_uzFile);
    m_uzFile = NULL;
    m_outputFolder = "";

    return (nRet == UNZ_OK);
}

//------------------------------------------------------------------------------

// simple interface
bool Unzipper::Unzip(bool bIgnoreFilePath)
{
    bool res = false;
    if (m_uzFile)
    {
        res = this->UnzipTo(m_outputFolder, bIgnoreFilePath);
    }
    return res;
}

//------------------------------------------------------------------------------

bool Unzipper::UnzipTo(::boost::filesystem::path folder, bool bIgnoreFilePath)
{
    if (!m_uzFile)
        return false;

    if (folder.empty())
    {
        folder = m_outputFolder;
    }
    if(!Unzipper::CreateFolder(folder))
    {
        return false;
    }

    if (this->GetFileCount() == 0)
        return false;

    if (! this->GotoFirstFile())
        return false;

    // else
    do
    {
        if (! this->UnzipFile(folder, bIgnoreFilePath))
            return false;
    }
    while (this->GotoNextFile());

    return true;
}

//------------------------------------------------------------------------------

// static version
bool Unzipper::Unzip(::boost::filesystem::path fileName, ::boost::filesystem::path folder, bool bIgnoreFilePath)
{
    Unzipper unz;
    if (!unz.OpenZip(fileName))
        return false;

    return unz.UnzipTo(folder, bIgnoreFilePath);
}

//------------------------------------------------------------------------------

// extended interface
bool Unzipper::OpenZip(::boost::filesystem::path filePath)
{
    this->CloseZip();

    if (!::boost::filesystem::exists(filePath))
        return false;

    m_uzFile = unzOpen(filePath.string().c_str());

    if (m_uzFile)
    {
        m_outputFolder = ::boost::filesystem::change_extension(filePath, "");
    }

    return (m_uzFile != NULL);
}

//------------------------------------------------------------------------------

bool Unzipper::SetOutputFolder(::boost::filesystem::path folder)
{
    bool res = Unzipper::CreateFolder(folder);

    if(::boost::filesystem::is_directory(folder))
    {
        m_outputFolder = folder;
        res = true;
    }
    return res;
}

//------------------------------------------------------------------------------

int Unzipper::GetFileCount()
{
    int nbFile = 0;
    if (m_uzFile)
    {
        unz_global_info info;
        if (unzGetGlobalInfo(m_uzFile, &info) == UNZ_OK)
        {
            nbFile = (int)info.number_entry;
        }
    }
    return nbFile;
}

//------------------------------------------------------------------------------

bool Unzipper::GetFileInfo(int nFile, UnZipFileInfo& info)
{
    if (!m_uzFile)
        return false;

    if (!this->GotoFile(nFile))
        return false;

    return this->GetFileInfo(info);
}

//------------------------------------------------------------------------------

bool Unzipper::UnzipFile(int nFile, ::boost::filesystem::path folder, bool bIgnoreFilePath)
{
    bool res = false;
    if (m_uzFile)
    {
        if (folder.empty())
        {
            folder = m_outputFolder;
        }

        if (this->GotoFile(nFile))
        {
            res = this->UnzipFile(folder, bIgnoreFilePath);
        }
    }
    return res;
}

//------------------------------------------------------------------------------

bool Unzipper::GotoFirstFile()
{
    bool res = false;
    if (m_uzFile)
    {
        res = (unzGoToFirstFile(m_uzFile) == UNZ_OK);
    }
    return res;
}

//------------------------------------------------------------------------------

bool Unzipper::GotoNextFile()
{
    bool res = false;
    if (m_uzFile)
    {
        res = (unzGoToNextFile(m_uzFile) == UNZ_OK);
    }
    return res;
}

//------------------------------------------------------------------------------

bool Unzipper::GetFileInfo(UnZipFileInfo& info)
{
    bool res = false;
    if (m_uzFile)
    {
        unz_file_info uzfi;
        memset( &uzfi, 0, sizeof(uzfi) );

        const unsigned int MAX_COMMENT = 256;
        char fileName[MAX_PATH + 1];
        char comment[MAX_COMMENT + 1];
        if (unzGetCurrentFileInfo(m_uzFile, &uzfi, fileName, MAX_PATH, NULL, 0, comment, MAX_COMMENT) == UNZ_OK)
        {
            // copy across
            info.dwVersion = uzfi.version;
            info.dwVersionNeeded = uzfi.version_needed;
            info.dwFlags = uzfi.flag;
            info.dwCompressionMethod = uzfi.compression_method;
            info.dwDosDate = uzfi.dosDate;
            info.dwCRC = uzfi.crc;
            info.dwCompressedSize = uzfi.compressed_size;
            info.dwUncompressedSize = uzfi.uncompressed_size;
            info.dwInternalAttrib = uzfi.internal_fa;
            info.dwExternalAttrib = uzfi.external_fa;
            info.comment = std::string(comment);
            info.fileName = ::boost::filesystem::path(fileName);
            info.fileDate.tm_year = uzfi.tmu_date.tm_year - 1900;
            info.fileDate.tm_mon  = uzfi.tmu_date.tm_mon;
            info.fileDate.tm_mday = uzfi.tmu_date.tm_mday;
            info.fileDate.tm_hour = uzfi.tmu_date.tm_hour - 1;
            info.fileDate.tm_min  = uzfi.tmu_date.tm_min;
            info.fileDate.tm_sec  = uzfi.tmu_date.tm_sec;

            // is it a folder?
#ifdef _WIN32
            info.bFolder = ((info.dwExternalAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
#else
            info.bFolder = S_ISDIR( info.dwExternalAttrib );
#endif
            res = true;
        }
    }
    return res;
}

//------------------------------------------------------------------------------

bool Unzipper::UnzipFile(::boost::filesystem::path folder, bool bIgnoreFilePath)
{
    if (!m_uzFile)
        return false;

    if (folder.empty())
    {
        folder = m_outputFolder;
    }

    if(!Unzipper::CreateFolder(folder))
    {
        return false;
    }

    UnZipFileInfo info;
    this->GetFileInfo(info);

    // if the item is a folder then create it and return 'true'
    if (info.bFolder)
    {
        ::boost::filesystem::path folderPath = folder / info.fileName;
        bool res = Unzipper::CreateFolder(folderPath);
        if(res)
        {
            res = Unzipper::SetFileModTime(folderPath, info.fileDate);
        }
        return res;
    }

    if (bIgnoreFilePath)
    {
        info.fileName =  ::boost::filesystem::path(info.fileName.filename());
    }

    ::boost::filesystem::path filePath = folder / info.fileName;

    if (!Unzipper::CreateFilePath(filePath))
        return false;

    std::ofstream outputFile;
    outputFile.open(filePath.string().c_str(), std::ios::out | std::ios::binary);
    if (!outputFile.is_open())
    {
        return false;
    }
    if (unzOpenCurrentFile(m_uzFile) != UNZ_OK)
        return false;

    // read the file and output
    int nRet = UNZ_OK;
    char pBuffer[BUFFERSIZE];

    do
    {
        nRet = unzReadCurrentFile(m_uzFile, pBuffer, BUFFERSIZE);
        if (nRet > 0)
        {
            outputFile.write(pBuffer, nRet);
        }
    }
    while (nRet > 0);

    outputFile.close();
    unzCloseCurrentFile(m_uzFile);

    if (nRet == UNZ_OK)
    {
        Unzipper::SetFileModTime(filePath, info.fileDate);
    }
    return (nRet == UNZ_OK);
}

//------------------------------------------------------------------------------

bool Unzipper::GotoFile(int nFile)
{
    bool res = false;
    if (m_uzFile)
    {
        if (nFile >= 0 || nFile < GetFileCount())
        {
            this->GotoFirstFile();
            while (nFile--)
            {
                res = this->GotoNextFile();
                if (!res)
                {
                    break;
                }
            }
        }
    }
    return res;
}

//------------------------------------------------------------------------------

bool Unzipper::GotoFile(::boost::filesystem::path fileName, bool bIgnoreFilePath)
{
    if (!m_uzFile)
        return false;

    // try the simple approach
    if (unzLocateFile(m_uzFile, fileName.string().c_str(), 2) == UNZ_OK)
    {
        return true;
    }
    else if (bIgnoreFilePath)
    {
        // brute force way
        if (unzGoToFirstFile(m_uzFile) != UNZ_OK)
            return false;

        UnZipFileInfo info;
        do
        {
            if (!this->GetFileInfo(info))
                return false;


            if(fileName.filename() == info.fileName.filename() )
            {
                return true;
            }
        }
        while (unzGoToNextFile(m_uzFile) == UNZ_OK);
    }

    // else
    return false;
}

//------------------------------------------------------------------------------

bool Unzipper::CreateFolder(::boost::filesystem::path folder)
{
    bool res = ::boost::filesystem::exists(folder);
    if (!res)
    {
        res = ::boost::filesystem::create_directories(folder);
    }
    return res;
}

//------------------------------------------------------------------------------

bool Unzipper::CreateFilePath(::boost::filesystem::path filePath)
{
    return Unzipper::CreateFolder(filePath.parent_path());
}

//------------------------------------------------------------------------------

bool Unzipper::SetFileModTime(::boost::filesystem::path filePath, std::tm fileDate)
{
    bool bRes = false;
    std::time_t lastWriteTime = std::mktime( &fileDate );
    if(lastWriteTime>=0)
    {
        ::boost::filesystem::last_write_time(filePath, lastWriteTime);
        bRes = true;
    }
    return bRes;
}

//------------------------------------------------------------------------------

} // namespace microzip
