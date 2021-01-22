/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwAtomsBoostIO/config.hpp"
#include "fwAtomsBoostIO/types.hpp"

#include <core/macros.hpp>

#include <filesystem>
#include <sstream>

namespace fwAtoms
{
class Base;
}
namespace fwZip
{
class IWriteArchive;
}

namespace fwAtomsBoostIO
{

class FWATOMSBOOSTIO_CLASS_API Writer
{

public:

    /// Defines writer version.
    FWATOMSBOOSTIO_API static const std::string s_VERSION;

    /// Defines key to retrieve fwAtoms version from file.
    FWATOMSBOOSTIO_API static const std::string s_ATOMS_VERSION_KEY;

    /// Defines key to retrieve writer version from file.
    FWATOMSBOOSTIO_API static const std::string s_WRITER_VERSION_KEY;

    Writer(const SPTR(::fwAtoms::Base)& atom) :
        m_atom(atom)
    {
    }

    /**
     * @brief Writes the archive in the given file name to the given format.
     * @param _archive archive to save.
     * @param _rootFilename filename of the saved archive.
     * @param _format format of the saved archive (JSON/XML).
     * @return The folder name used to save data.
     */
    FWATOMSBOOSTIO_API std::filesystem::path write( const SPTR(::fwZip::IWriteArchive)_archive,
                                                    const std::filesystem::path& _rootFilename = "root.json",
                                                    FormatType _format                         = JSON ) const;

protected:

    SPTR(::fwAtoms::Base) m_atom;

};

}
