/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2015 IHU Strasbourg
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

#include "io/patch/structural/data/Mesh/V1ToV3.hpp"

#include <atoms/Map.hpp>
#include <atoms/String.hpp>

#include <io/atoms/patch/StructuralCreatorDB.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace Mesh
{

V1ToV3::V1ToV3() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "::sight::data::Mesh";
    m_targetClassname = "::sight::data::Mesh";
    m_originVersion   = "1";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V1ToV3::~V1ToV3()
{
}

// ----------------------------------------------------------------------------

V1ToV3::V1ToV3(const V1ToV3& cpy) :
    io::atoms::patch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV3::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    io::atoms::patch::helper::Object helper(current);
    helper.addAttribute("point_tex_coords", sight::atoms::Object::sptr());
    helper.addAttribute("cell_tex_coords", sight::atoms::Object::sptr());
    helper.addAttribute("array_map", sight::atoms::Map::New());
}

} // namespace Mesh

} // namespace data

} // namespace sight::io::patch::structural
