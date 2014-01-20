/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_WRITEZIPARCHIVE_HPP__
#define __FWZIP_WRITEZIPARCHIVE_HPP__

#include <ostream>

#include <boost/make_shared.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/make_shared.hpp>

#include <fwCore/macros.hpp>

#include "fwZip/config.hpp"
#include "fwZip/IWriteArchive.hpp"


namespace fwZip
{

/**
 * @brief   This class defines functions to write a file in a zip archive.
 * @class   WriteZipArchive
 */
class FWZIP_CLASS_API WriteZipArchive : public IWriteArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((WriteZipArchive)(IWriteArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           ::boost::make_shared<WriteZipArchive> );

    /**
     * @brief Constructors. Initializes archive path.
     *
     */
    FWZIP_API WriteZipArchive( const ::boost::filesystem::path &archive );

    FWZIP_API ~WriteZipArchive();

    /**
     * @brief Creates a new file entry in archive and returns output stream for this file.
     * @param path file in archive.
     * @return output stream of file entry in archive.
     *
     * @throw ::fwZip::exception::Write if archive cannot be opened.
     * @note Last output stream is automatically flushed before creation of new file entry in zip archive.
     */
    FWZIP_API SPTR(std::ostream) createFile(const ::boost::filesystem::path &path);

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

    ::boost::filesystem::path m_archive;
};

}


#endif /* __FWZIP_WRITEZIPARCHIVE_HPP__ */

