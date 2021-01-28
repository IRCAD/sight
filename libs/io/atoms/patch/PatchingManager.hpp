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

#include "io/atoms/config.hpp"
#include "io/atoms/patch/VersionsGraph.hpp"

#include <core/macros.hpp>

namespace sight::atoms
{
class Object;
}

namespace sight::io::atoms::patch
{

/**
 * @brief Manages object patching.
 *
 * To be used, PatchingManager requires that available contexts and versions are loaded into
 * the graph of versions through the VersionsManager.
 */
class IO_ATOMS_CLASS_API PatchingManager
{
public:

    /// Default constructor
    IO_ATOMS_API PatchingManager(SPTR(sight::atoms::Object)object);

    /// Default destructor
    IO_ATOMS_API ~PatchingManager();

    /**
     * @brief Transforms the object to the given version.
     * @pre Given version must be known by the VersionsManager
     *
     * @throw io::atoms::patch::exceptions::MissingInformation if an information about patching is missing
     * @throw io::atoms::patch::exceptions::ImpossibleConversion if there is no patch available to transform an object
     */
    IO_ATOMS_API SPTR(sight::atoms::Object) transformTo(const std::string& newVersion);

protected:

    /// Object that we want to patch
    SPTR(sight::atoms::Object) m_object;
};

}
