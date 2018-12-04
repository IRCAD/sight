/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWATOMSBOOSTIO_WRITER_HPP__
#define __FWATOMSBOOSTIO_WRITER_HPP__

#include <sstream>

#include <boost/filesystem/path.hpp>

#include <fwCore/macros.hpp>

#include "fwAtomsBoostIO/types.hpp"
#include "fwAtomsBoostIO/config.hpp"

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

    Writer(const SPTR(::fwAtoms::Base)& atom) : m_atom(atom)
    {
    }

    FWATOMSBOOSTIO_API void write( const SPTR(::fwZip::IWriteArchive)& archive,
                                   const ::boost::filesystem::path& rootFilename = "root.json",
                                   FormatType format = JSON ) const;

protected:

    SPTR(::fwAtoms::Base) m_atom;

};

}

#endif /* __FWATOMSBOOSTIO_WRITER_HPP__ */

