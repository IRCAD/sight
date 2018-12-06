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

#ifndef __FWZIP_IWRITEARCHIVE_HPP__
#define __FWZIP_IWRITEARCHIVE_HPP__

#include <fwCore/macros.hpp>

#include <boost/filesystem/path.hpp>

#include <ostream>

namespace fwZip
{

/**
 * @brief   This interface defines functions to write a file in an archive.
 */
class IWriteArchive
{

public:

    fwCoreBaseClassDefinitionsMacro((IWriteArchive))

    virtual ~IWriteArchive()
    {
    }

    /**
     * @brief Creates a new file entry in archive and returns output stream for this file.
     * @param path file in archive.
     * @return output stream of file entry in archive.
     */
    virtual SPTR(std::ostream) createFile(const ::boost::filesystem::path &path) = 0;

    /**
     * @brief Writes source file in archive.
     * @param sourceFile source file.
     * @param path file in archive.
     */
    virtual void putFile(const ::boost::filesystem::path& sourceFile, const ::boost::filesystem::path& path) = 0;

    /**
     * @brief Creates a folder in archive.
     * @param path folder to create in archive.
     */
    virtual bool createDir(const ::boost::filesystem::path& path) = 0;

    /**
     * @brief Returns archive path.
     */
    virtual const ::boost::filesystem::path getArchivePath() const = 0;
};

}

#endif /* __FWZIP_IWRITEARCHIVE_HPP__ */

