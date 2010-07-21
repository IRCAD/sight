#ifndef ZIPPER_HPP_
#define ZIPPER_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <zip.h>
#ifdef _WIN32
#include <iowin32.h>
#endif

//TODO: minizip/microzip as external dependency
//#include "export.hpp"
#define MICROZIP_API
#define MICROZIP_CLASS_API

namespace microzip
{

struct ZipFileInfo
{
    int nFileCount;
    int nFolderCount;
    unsigned long dwUncompressedSize;
};

class MICROZIP_CLASS_API Zipper
{
public:
    MICROZIP_API Zipper();

    MICROZIP_API Zipper(::boost::filesystem::path filePath, ::boost::filesystem::path rootFolder, bool bAppend = false);

    MICROZIP_API virtual ~Zipper();

    MICROZIP_API static bool ZipFile(::boost::filesystem::path filePath, ::boost::filesystem::path zipFilePath = "", int level = Z_DEFAULT_COMPRESSION); // saves as same name with .zip
    MICROZIP_API static bool ZipFolder(::boost::filesystem::path folderPath, ::boost::filesystem::path zipFilePath = "", bool bIgnoreFilePath=false, int level = Z_DEFAULT_COMPRESSION); // saves as same name with .zip

    /*
     *  The compression level must be Z_DEFAULT_COMPRESSION, or between 0 and 9:
     *  1 gives best speed,
     *  9 gives best compression,
     *  0 gives no compression at all (the input data is simply copied a block at a time).
     *  Z_DEFAULT_COMPRESSION requests a default compromise between speed and compression (currently equivalent to level 6).
     */
    MICROZIP_API bool AddFileToZip(::boost::filesystem::path filePath, bool bIgnoreFilePath = false, int level = Z_DEFAULT_COMPRESSION );
    MICROZIP_API bool AddFileToZip(::boost::filesystem::path filePath, ::boost::filesystem::path relFolderPath, int level = Z_DEFAULT_COMPRESSION); // replaces path info from filePath with folder
    MICROZIP_API bool AddFolderToZip(::boost::filesystem::path folderPath, bool bIgnoreFilePath = false, int level = Z_DEFAULT_COMPRESSION);

    // extended interface
    MICROZIP_API bool OpenZip(::boost::filesystem::path filePath, ::boost::filesystem::path rootFolder = "", bool bAppend = false);
    MICROZIP_API bool CloseZip(); // for multiple reuse
    MICROZIP_API void GetFileInfo(ZipFileInfo& info);

    MICROZIP_API ::boost::filesystem::path relative_path( ::boost::filesystem::path const path, ::boost::filesystem::path const base);

protected:

    void* m_uzFile;
    ::boost::filesystem::path m_rootFolder;
    ZipFileInfo m_info;

    unsigned long getFileAttributes(::boost::filesystem::path const filePath);
};

}

#endif // ZIPPER_HPP_
