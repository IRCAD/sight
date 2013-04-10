/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_READZIPARCHIVE_HPP__
#define __FWZIP_READZIPARCHIVE_HPP__

#include <istream>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/categories.hpp>  // source_tag

#include <boost/filesystem/path.hpp>

#include <boost/make_shared.hpp>

#include <fwCore/macros.hpp>

#include "fwZip/config.hpp"
#include "fwZip/IReadArchive.hpp"


namespace fwZip
{

class ZipSource
{
public:
    typedef char char_type;
    typedef ::boost::iostreams::source_tag  category;

    ZipSource( void * zd ) : m_zipDescriptor(zd){}

    std::streamsize read(char* s, std::streamsize n);

protected:
    void *m_zipDescriptor;
};


/**
 * @brief   This class defines functions to read a file in a zip archive.
 * @class   ReadZipArchive.
 */
class FWZIP_CLASS_API ReadZipArchive : public IReadArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ReadZipArchive)(IReadArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           ::boost::make_shared< ReadZipArchive >);

    /*
     * @brief Constructors. Initializes archive path, zip descriptor and zip stream.
     *
     * @throw ::fwZip::exception::Read if archive doesn't exist.
     * @throw ::fwZip::exception::Read if archive cannot be opened.
     */
    FWZIP_API ReadZipArchive( const ::boost::filesystem::path &archive );

    /// Destructor. Close ZipFile.
    FWZIP_API ~ReadZipArchive();

    /**
     * @brief Returns input stream for the file in current archive (zip).
     * @param path file in archive.
     * @return input stream from request file.
     *
     * @throw ::fwZip::exception::Read if file doesn't exist in archive.
     * @throw ::fwZip::exception::Read if cannot retrieve file in archive.
     */
    FWZIP_API std::istream& getFile(const ::boost::filesystem::path &path);

    /**
     * @brief Returns archive path.
     */
    FWZIP_API const ::boost::filesystem::path getArchivePath() const;

protected:

    /**
     * @brief Returns true if path exists in archive. If the file is found, it becomes the current file.
     */
    FWZIP_API bool exists(const ::boost::filesystem::path &path);

    ::boost::filesystem::path m_archive;
    void *m_zipDescriptor;
    ::boost::iostreams::stream_buffer< ZipSource > m_streambuf;
    std::istream m_istream;
};

}


#endif /* __FWZIP_READZIPARCHIVE_HPP__ */

