/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "io/patch/config.hpp"

#include <io/atoms/patch/IStructuralPatch.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace Mesh
{

/// Structural patch to convert a data::Mesh from version '3' to version '4'.
class IO_PATCH_CLASS_API V3ToV4 : public io::atoms::patch::IStructuralPatch
{
public:

    SIGHT_DECLARE_CLASS(V3ToV4, patch::structural::data::Mesh::V3ToV4, new V3ToV4);

    /// Constructor
    IO_PATCH_API V3ToV4();

    /// Destructor
    IO_PATCH_API ~V3ToV4();

    /// Copy constructor
    IO_PATCH_API V3ToV4(const V3ToV4& cpy);

    /**
     * @brief Applies patch
     *
     * Added new map of additional array.
     */
    IO_PATCH_API virtual void apply(
        const sight::atoms::Object::sptr& previous,
        const sight::atoms::Object::sptr& current,
        io::atoms::patch::IPatch::NewVersionsType& newVersions
    ) override;
};

} // namespace Mesh

} // namespace data

} // namespace sight::io::patch::structural
