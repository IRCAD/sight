/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/atoms/config.hpp"
#include "io/atoms/types.hpp"

#include <core/macros.hpp>

#include <filesystem>

namespace sight::atoms
{

class Base;

}
namespace sight::io::zip
{

class IReadArchive;

}

namespace sight::io::atoms
{

class IO_ATOMS_CLASS_API Reader
{
public:

    IO_ATOMS_API SPTR(sight::atoms::Base) read(
        const SPTR(io::zip::IReadArchive) & archive,
        const std::filesystem::path& rootFilename = "root.json",
        FormatType format                         = JSON
    ) const;

protected:

    SPTR(sight::atoms::Base) m_atom;
};

}
