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

#include "fwAtomsPatch/config.hpp"
#include "fwAtomsPatch/VersionsGraph.hpp"

#include <core/macros.hpp>

namespace sight::atoms
{
class Object;
}

namespace fwAtomsPatch
{

/**
 * @brief Manages object patching.
 *
 * To be used, PatchingManager requires that available contexts and versions are loaded into
 * the graph of versions through the VersionsManager.
 */
class FWATOMSPATCH_CLASS_API PatchingManager
{
public:

    /// Default constructor
    FWATOMSPATCH_API PatchingManager(SPTR(atoms::Object)object);

    /// Default destructor
    FWATOMSPATCH_API ~PatchingManager();

    /**
     * @brief Transforms the object to the given version.
     * @pre Given version must be known by the VersionsManager
     *
     * @throw ::fwAtomsPatch::exceptions::MissingInformation if an information about patching is missing
     * @throw ::fwAtomsPatch::exceptions::ImpossibleConversion if there is no patch available to transform an object
     */
    FWATOMSPATCH_API SPTR(atoms::Object) transformTo(const std::string& newVersion);

protected:

    /// Object that we want to patch
    SPTR(atoms::Object) m_object;
};

}
