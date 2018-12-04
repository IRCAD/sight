/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

