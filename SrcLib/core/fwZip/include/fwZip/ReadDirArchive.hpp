/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_READDIRARCHIVE_HPP__
#define __FWZIP_READDIRARCHIVE_HPP__

#include <fstream>

#include <boost/filesystem/path.hpp>

#include <boost/make_shared.hpp>

#include <fwCore/macros.hpp>

#include "fwZip/config.hpp"
#include "fwZip/IReadArchive.hpp"


namespace fwZip
{

/**
 * @brief   This class defines functions to read a file in a file system archive.
 * @class   ReadDirArchive
 */
class FWZIP_CLASS_API ReadDirArchive : public IReadArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ReadDirArchive)(IReadArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           ::boost::make_shared< ReadDirArchive >);

    /// Constructors. Initializes archive path.
    FWZIP_API ReadDirArchive( const ::boost::filesystem::path &archive );

    /// Destructor. Close automatically last input file stream.
    FWZIP_API ~ReadDirArchive();

    /**
     * @brief Returns input stream for the file in current archive.
     * @param path file in archive.
     * @return input stream from request file.
     *
     * @throw ::fwZip::exception::Read if file doesn't exist in archive.
     */
    FWZIP_API SPTR(std::istream) getFile(const ::boost::filesystem::path &path);

    /**
     * @brief Returns archive path.
     */
    FWZIP_API const ::boost::filesystem::path getArchivePath() const;

    IReadArchive::sptr clone() const
    {
        return ReadDirArchive::New(m_archive);
    }

protected:

    /**
     * @brief Returns true if path exists in archive.
     */
    FWZIP_API bool exists(const ::boost::filesystem::path &path);

    ::boost::filesystem::path m_archive;
};

}


#endif /* __FWZIP_READDIRARCHIVE_HPP__ */

