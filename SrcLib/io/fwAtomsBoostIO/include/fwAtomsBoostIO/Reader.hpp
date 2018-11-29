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

#ifndef __FWATOMSBOOSTIO_READER_HPP__
#define __FWATOMSBOOSTIO_READER_HPP__

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
class IReadArchive;
}

namespace fwAtomsBoostIO
{

class FWATOMSBOOSTIO_CLASS_API Reader
{

public:

    FWATOMSBOOSTIO_API SPTR(::fwAtoms::Base) read( const SPTR(::fwZip::IReadArchive)& archive,
                                                   const ::boost::filesystem::path& rootFilename = "root.json",
                                                   FormatType format = JSON ) const;

protected:

    SPTR(::fwAtoms::Base) m_atom;

};

}

#endif /* __FWATOMSBOOSTIO_READER_HPP__ */

