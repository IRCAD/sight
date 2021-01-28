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

#include "io/patch/structural/data/Image/V3ToV2.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/String.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace Image
{

V3ToV2::V3ToV2() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "::sight::data::Image";
    m_targetClassname = "::sight::data::Image";
    m_originVersion   = "3";
    m_targetVersion   = "2";

}

// ----------------------------------------------------------------------------

V3ToV2::~V3ToV2()
{
}

// ----------------------------------------------------------------------------

V3ToV2::V3ToV2( const V3ToV2& cpy ) :
    io::atoms::patch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V3ToV2::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    io::atoms::patch::helper::Object helper(current);

    helper.removeAttribute("pixel_format");
}

} // namespace Image

} // namespace data

} // namespace sight::io::patch::structural
