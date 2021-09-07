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
#include "io/atoms/patch/helper/Object.hpp"
#include "io/atoms/patch/IPatch.hpp"

#include <core/base.hpp>

#include <string>

namespace sight::atoms
{

class Object;

}

namespace sight::io::atoms::patch
{

/**
 * @brief Base class of all structural patches
 */
class IO_ATOMS_CLASS_API IStructuralPatch : public IPatch
{
public:

    SIGHT_DECLARE_CLASS(IStructuralPatch, IPatch);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    ///Constructor
    IO_ATOMS_API IStructuralPatch();

    ///Copy constructor
    IO_ATOMS_API IStructuralPatch(const IStructuralPatch& cpy);

    ///Destructor
    IO_ATOMS_API virtual ~IStructuralPatch();

    /**
     * @brief Applies the patch to the specified object
     *
     * @param previous object from which modifications will be applied
     * @param current object receiving modifications
     * @param newVersions map containing already patched sub objects
     *
     * @pre previous and current objects must have the same classname and version.
     */
    IO_ATOMS_API virtual void apply(
        const SPTR(sight::atoms::Object)& previous,
        const SPTR(sight::atoms::Object)& current,
        io::atoms::patch::IPatch::NewVersionsType& newVersions
    ) override;

    /// Returns the target classname of the object
    IO_ATOMS_API virtual const std::string& getTargetClassname() const;

    /// Returns the target version of the object
    IO_ATOMS_API virtual const std::string& getTargetVersion() const;

protected:

    /// Updates the version of the patched object
    IO_ATOMS_API void updateVersion(SPTR(sight::atoms::Object) current);

    /// Target type
    std::string m_targetClassname;

    /// Target version
    std::string m_targetVersion;
};

} // namespace sight::io::atoms::patch
