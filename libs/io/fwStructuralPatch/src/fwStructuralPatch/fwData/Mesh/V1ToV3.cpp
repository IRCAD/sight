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

#include "fwStructuralPatch/data/Mesh/V1ToV3.hpp"

#include <fwAtoms/Map.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/StructuralCreatorDB.hpp>

namespace fwStructuralPatch
{

namespace sight::data
{

namespace Mesh
{

V1ToV3::V1ToV3() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "data::Mesh";
    m_targetClassname = "data::Mesh";
    m_originVersion   = "1";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V1ToV3::~V1ToV3()
{
}

// ----------------------------------------------------------------------------

V1ToV3::V1ToV3( const V1ToV3& cpy ) :
    ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV3::apply( const ::fwAtoms::Object::sptr& previous,
                    const ::fwAtoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);
    helper.addAttribute("point_tex_coords", ::fwAtoms::Object::sptr() );
    helper.addAttribute("cell_tex_coords", ::fwAtoms::Object::sptr() );
    helper.addAttribute("array_map", ::fwAtoms::Map::New() );
}

} // namespace Mesh

} // namespace sight::data

} // namespace fwStructuralPatch
