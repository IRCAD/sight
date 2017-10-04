/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_READDIRARCHIVE_HPP__
#define __FWZIP_READDIRARCHIVE_HPP__

#include "fwZip/config.hpp"
#include "fwZip/IReadArchive.hpp"

#include <fwCore/macros.hpp>

#include <boost/filesystem/path.hpp>

#include <fstream>

namespace fwZip
{

/**
 * @brief   This class defines functions to read a file in a file system archive.
 */
class FWZIP_CLASS_API ReadDirArchive : public IReadArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ReadDirArchive)(IReadArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           std::make_shared< ReadDirArchive >)

    /// Constructors. Initializes archive path.
    FWZIP_API ReadDirArchive( const ::boost::filesystem::path& archive );

    /// Destructor. Close automatically last input file stream.
    FWZIP_API ~ReadDirArchive();

    /**
     * @brief Returns input stream for the file in current archive.
     * @param path file in archive.
     * @return input stream from request file.
     *
     * @throw ::fwZip::exception::Read if file doesn't exist in archive.
     */
    FWZIP_API SPTR(std::istream) getFile(const ::boost::filesystem::path &path) override;

    /**
     * @brief Returns archive path.
     */
    FWZIP_API const ::boost::filesystem::path getArchivePath() const override;

    //------------------------------------------------------------------------------

    IReadArchive::sptr clone() const override
    {
        return ReadDirArchive::New(m_archive);
    }

protected:

    ::boost::filesystem::path m_archive;
};

}

#endif /* __FWZIP_READDIRARCHIVE_HPP__ */

