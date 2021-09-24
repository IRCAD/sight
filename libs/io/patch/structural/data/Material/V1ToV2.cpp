/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/patch/structural/data/Material/V1ToV2.hpp"

#include <atoms/String.hpp>

#include <io/atoms/patch/StructuralCreatorDB.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace Material
{

V1ToV2::V1ToV2() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "sight::data::Material";
    m_targetClassname = "sight::data::Material";
    m_originVersion   = "1";
    m_targetVersion   = "2";
}

// ----------------------------------------------------------------------------

V1ToV2::~V1ToV2()
{
}

// ----------------------------------------------------------------------------

V1ToV2::V1ToV2(const V1ToV2& cpy) :
    io::atoms::patch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV2::apply(
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
    helper.addAttribute("diffuse_texture", sight::atoms::Object::sptr());
    helper.addAttribute("diffuse_texture_filtering", sight::atoms::String::New("LINEAR"));
    helper.addAttribute("diffuse_texture_wrapping", sight::atoms::String::New("REPEAT"));
}

} // namespace Material

} // namespace data

} // namespace sight::io::patch::structural
