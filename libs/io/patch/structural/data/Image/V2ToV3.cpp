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

#include "io/patch/structural/data/Image/V2ToV3.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace Image
{

V2ToV3::V2ToV3() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "::sight::data::Image";
    m_targetClassname = "::sight::data::Image";
    m_originVersion   = "2";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V2ToV3::~V2ToV3()
{
}

// ----------------------------------------------------------------------------

V2ToV3::V2ToV3(const V2ToV3& cpy) :
    io::atoms::patch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V2ToV3::apply(
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

    sight::atoms::Numeric::sptr components = current->getAttribute<sight::atoms::Numeric>("nb_components");
    size_t nbComponents                    = components->getValue<size_t>();
    if(nbComponents == 1)
    {
        helper.addAttribute("pixel_format", sight::atoms::String::New("GRAY_SCALE"));
    }
    else if(nbComponents == 3)
    {
        helper.addAttribute("pixel_format", sight::atoms::String::New("RGB"));
    }
    else if(nbComponents == 4)
    {
        helper.addAttribute("pixel_format", sight::atoms::String::New("RGBA"));
    }
    else
    {
        helper.addAttribute("pixel_format", sight::atoms::String::New("UNDEFINED"));
    }
}

} // namespace Image

} // namespace data

} // namespace sight::io::patch::structural
