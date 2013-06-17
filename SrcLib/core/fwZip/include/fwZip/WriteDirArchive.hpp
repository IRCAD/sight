/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_WRITEDIRARCHIVE_HPP__
#define __FWZIP_WRITEDIRARCHIVE_HPP__

#include <fstream>

#include <boost/filesystem/path.hpp>

#include <boost/make_shared.hpp>

#include <fwCore/macros.hpp>

#include "fwZip/config.hpp"
#include "fwZip/IWriteArchive.hpp"


namespace fwZip
{

/**
 * @brief   This class defines functions to write a file in a file system archive.
 * @class   WriteDirArchive.
 */
class FWZIP_CLASS_API WriteDirArchive : public IWriteArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((WriteDirArchive)(IWriteArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           ::boost::make_shared<WriteDirArchive> );

    /// Constructors. Initializes archive path and creates archive directories if doesn't exist.
    FWZIP_API WriteDirArchive( const ::boost::filesystem::path &archive );

    /// Destructor. Flush and close last output file stream.
    FWZIP_API ~WriteDirArchive();

    /**
     * @brief Creates a new file entry in archive and returns output stream for this file.
     * @param path file in archive.
     * @return output stream of file entry in archive.
     *
     * @note Creates all parent directories of the path in archive.
     */
    FWZIP_API SPTR(std::ostream) createFile(const ::boost::filesystem::path &path);

    /**
     * @brief Writes source file in archive. If possible, creates hard link otherwise copy source file in archive.
     * @param sourceFile source file.
     * @param path file in archive.
     *
     * @note Do nothing if path already exists in archive.
     */
    FWZIP_API void putFile(const ::boost::filesystem::path &sourceFile, const ::boost::filesystem::path &path);

    /**
     * @brief Creates a folder in archive.
     * @param path folder to create in archive.
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


#endif /* __FWZIP_WRITEDIRARCHIVE_HPP__ */

