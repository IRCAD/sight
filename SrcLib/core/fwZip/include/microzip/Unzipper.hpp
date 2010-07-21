#ifndef UNZIPPER_HPP_
#define UNZIPPER_HPP_

#include <string>
#include <ctime>

#include <boost/filesystem/path.hpp>

#include <unzip.h>
#ifdef _WIN32
#include <iowin32.h>
#endif

#ifndef MAX_PATH
#       define MAX_PATH     (256)
#endif

//TODO: minizip/microzip as external dependency
//#include "export.hpp"
#define MICROZIP_API
#define MICROZIP_CLASS_API

namespace microzip
{

struct UnZipFileInfo
{
    ::boost::filesystem::path fileName;
    std::string comment;

    unsigned long dwVersion;
    unsigned long dwVersionNeeded;
    unsigned long dwFlags;
    unsigned long dwCompressionMethod;
    unsigned long dwDosDate;
    unsigned long dwCRC;
    unsigned long dwCompressedSize;
    unsigned long dwUncompressedSize;
    unsigned long dwInternalAttrib;
    unsigned long dwExternalAttrib;
    bool          bFolder;
    std::tm       fileDate;
};

class MICROZIP_CLASS_API Unzipper
{
public:
    MICROZIP_API Unzipper( );
    MICROZIP_API Unzipper(::boost::filesystem::path filePath );
    MICROZIP_API virtual ~Unzipper();

    // simple interface
    MICROZIP_API static bool Unzip(::boost::filesystem::path fileName, ::boost::filesystem::path folder = "", bool bIgnoreFilePath = false);

    // works with prior opened zip
    MICROZIP_API bool Unzip(bool bIgnoreFilePath = false); // unzips to output folder or sub folder with zip name
    MICROZIP_API bool UnzipTo(::boost::filesystem::path folder, bool bIgnoreFilePath = false); // unzips to specified folder

    // extended interface
    MICROZIP_API bool OpenZip(::boost::filesystem::path filePath);
    MICROZIP_API bool CloseZip(); // for multiple reuse
    MICROZIP_API bool SetOutputFolder(::boost::filesystem::path folder); // will try to create

    // unzip by file index
    MICROZIP_API int GetFileCount();
    MICROZIP_API bool GetFileInfo(int nFile, UnZipFileInfo& info);
    MICROZIP_API bool UnzipFile(int nFile, ::boost::filesystem::path folder = "", bool bIgnoreFilePath = false);

    // unzip current file
    MICROZIP_API bool GotoFirstFile();
    MICROZIP_API bool GotoNextFile();
    MICROZIP_API bool GetFileInfo(UnZipFileInfo& info);
    MICROZIP_API bool UnzipFile(::boost::filesystem::path folder = "", bool bIgnoreFilePath = false);

    // helpers
    MICROZIP_API bool GotoFile(::boost::filesystem::path fileName, bool bIgnoreFilePath = true);
    MICROZIP_API bool GotoFile(int nFile);

protected:
    void* m_uzFile;
    ::boost::filesystem::path m_outputFolder;

    static bool CreateFolder(::boost::filesystem::path folder);
    static bool CreateFilePath(::boost::filesystem::path filePath);
    static bool SetFileModTime(::boost::filesystem::path filePath, std::tm fileDate);

};

}

#endif // UNZIPPER_HPP_
