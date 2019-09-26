/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "fwZip/config.hpp"
#include "fwZip/IReadArchive.hpp"

#include <fwCore/macros.hpp>

#include <boost/filesystem/path.hpp>

#include <istream>

namespace fwZip
{

/**
 * @brief   This class defines functions to read a file in a zip archive.
 */
class FWZIP_CLASS_API ReadZipArchive : public IReadArchive
{

public:
    fwCoreClassMacro(ReadZipArchive, IReadArchive);

    //------------------------------------------------------------------------------

    static sptr New(const ::boost::filesystem::path& archive)
    {
        return std::make_shared< ReadZipArchive >(archive);
    }

    /*
     * @brief Constructors. Initializes archive path, zip descriptor and zip stream.
     *
     * @throw ::fwZip::exception::Read if archive doesn't exist.
     * @throw ::fwZip::exception::Read if archive cannot be opened.
     */
    FWZIP_API ReadZipArchive( const ::boost::filesystem::path& archive );

    /**
     * @brief Returns input stream for the file in current archive (zip).
     * @param path file in archive.
     * @return input stream from request file.
     *
     * @throw ::fwZip::exception::Read if file doesn't exist in archive.
     * @throw ::fwZip::exception::Read if cannot retrieve file in archive.
     */
    FWZIP_API SPTR(std::istream) getFile(const ::boost::filesystem::path& path) override;

    /**
     * @brief Returns comment from the current archive (zip).
     * @param path file in archive.
     * @return comment
     */
    FWZIP_API std::string getComment();

    /**
     * @brief Returns archive path.
     */
    FWZIP_API const ::boost::filesystem::path getArchivePath() const override;

    //------------------------------------------------------------------------------

    IReadArchive::sptr clone() const override
    {
        return ReadZipArchive::New(m_archive);
    }

protected:

    ::boost::filesystem::path m_archive;
};

}
