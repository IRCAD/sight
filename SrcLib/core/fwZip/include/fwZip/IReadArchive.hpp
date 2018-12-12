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

#ifndef __FWZIP_IREADARCHIVE_HPP__
#define __FWZIP_IREADARCHIVE_HPP__

#include <fwCore/macros.hpp>

#include <boost/filesystem/path.hpp>

#include <istream>

namespace fwZip
{

/**
 * @brief   This interface defines functions to read a file in an archive.
 */
class IReadArchive
{

public:

    fwCoreBaseClassDefinitionsMacro((IReadArchive))

    virtual ~IReadArchive()
    {
    }

    /**
     * @brief Returns input stream for the file in current archive.
     * @param path file in archive.
     * @return input stream from request file.
     */
    virtual SPTR(std::istream) getFile(const ::boost::filesystem::path &path) = 0;

    /**
     * @brief Returns archive path.
     */
    virtual const ::boost::filesystem::path getArchivePath() const = 0;

    virtual IReadArchive::sptr clone() const = 0;
};

}

#endif /* __FWZIP_IREADARCHIVE_HPP__ */

