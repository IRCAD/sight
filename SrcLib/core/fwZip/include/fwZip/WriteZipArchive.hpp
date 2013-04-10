/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_WRITEZIPARCHIVE_HPP__
#define __FWZIP_WRITEZIPARCHIVE_HPP__

#include <ostream>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/categories.hpp>  // sink_tag

#include <boost/filesystem/path.hpp>

#include <boost/make_shared.hpp>

#include <fwCore/macros.hpp>

#include "fwZip/config.hpp"
#include "fwZip/IWriteArchive.hpp"


namespace fwZip
{

class ZipSink
{
public:
    typedef char char_type;
    typedef ::boost::iostreams::sink_tag  category;

    ZipSink( void * zd ) : m_zipDescriptor(zd){}

    std::streamsize write(const char* s, std::streamsize n);

protected:
    void *m_zipDescriptor;
};


/**
 * @brief   This class defines functions to write a file in a zip archive.
 * @class   WriteZipArchive.
 */
class FWZIP_CLASS_API WriteZipArchive : public IWriteArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((WriteZipArchive)(IWriteArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           ::boost::make_shared<WriteZipArchive> );

    /**
     * @brief Constructors. Initializes archive path, zip descriptor and zip stream.
     *
     * @throw ::fwZip::exception::Write if archive already exists.
     * @throw ::fwZip::exception::Write if archive cannot be opened.
     */
    FWZIP_API WriteZipArchive( const ::boost::filesystem::path &archive );

    /// Destructor. Flush current output stream and close the current file in the zip file.
    FWZIP_API ~WriteZipArchive();

    /**
     * @brief Creates a new file entry in archive and returns output stream for this file.
     * @param path file in archive.
     * @return output stream of file entry in archive.
     *
     * @note Last output stream is automatically flushed before creation of new file entry in zip archive.
     */
    FWZIP_API std::ostream& createFile(const ::boost::filesystem::path &path);

    /**
     * @brief Writes source file in archive.
     * @param sourceFile source file.
     * @param path file in archive.
     *
     * @throw ::fwZip::exception::Read if source file cannot be opened.
     */
    FWZIP_API void putFile(const ::boost::filesystem::path &sourceFile, const ::boost::filesystem::path &path);

    /**
     * @brief Creates a folder in archive.
     * @param path folder to create in archive.
     *
     * @todo: file attribute isn't correctly set in zip archive.
     */
    FWZIP_API bool createDir(const ::boost::filesystem::path &path);

    /**
     * @brief Returns archive path.
     */
    FWZIP_API const ::boost::filesystem::path getArchivePath() const;

protected:

    /*
     * @brief  Open a file in the zip archive for writing
     * @note Z_BEST_SPEED compression level for '.raw' files,
     *       Z_NO_COMPRESSION for 'raw.gz', Z_DEFAULT_COMPRESSION otherwise.
     */
    FWZIP_API int openFile(const ::boost::filesystem::path &path);

    /// Close the current file in the zip archive.
    FWZIP_API void closeFile();

    ::boost::filesystem::path m_archive;
    void *m_zipDescriptor;
    ::boost::iostreams::stream_buffer< ZipSink > m_streambuf;
    std::ostream m_ostream;
};

}


#endif /* __FWZIP_WRITEZIPARCHIVE_HPP__ */

