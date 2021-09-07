/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
 * Copyright (C) 2015-2016 IHU Strasbourg
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

#include "io/patch/structural/data/Material/V3ToV4.hpp"

#include <atoms/Boolean.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <io/atoms/patch/StructuralCreatorDB.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace Material
{

V3ToV4::V3ToV4() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "::sight::data::Material";
    m_targetClassname = "::sight::data::Material";
    m_originVersion   = "3";
    m_targetVersion   = "4";
}

// ----------------------------------------------------------------------------

V3ToV4::~V3ToV4()
{
}

// ----------------------------------------------------------------------------

V3ToV4::V3ToV4(const V3ToV4& cpy) :
    io::atoms::patch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V3ToV4::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    io::atoms::patch::helper::Object helper(current);

    // Retrieve and remove lighting attribute, then update shading mode
    sight::atoms::Boolean::sptr lighting = previous->getAttribute<sight::atoms::Boolean>("lighting");
    bool bLighting                       = lighting->getValue();

    helper.removeAttribute("lighting");

    if(!bLighting)
    {
        helper.replaceAttribute("shading_mode", sight::atoms::String::New("AMBIENT"));
    }

    // Switch diffuse and ambient
    sight::atoms::Object::sptr ambient = current->getAttribute<sight::atoms::Object>("ambient");
    sight::atoms::Object::sptr diffuse = current->getAttribute<sight::atoms::Object>("diffuse");

    // Replace diffuse by previous ambient
    helper.replaceAttribute("diffuse", ambient);
    helper.replaceAttribute("ambient", diffuse);

    // Set ambient to 0.05f

    sight::atoms::Sequence::sptr ambientSeq = diffuse->getAttribute<sight::atoms::Sequence>("rgba");
    sight::atoms::Numeric::sptr r           = std::dynamic_pointer_cast<sight::atoms::Numeric>((*ambientSeq)[0]);
    r->setFromString(".05");
    sight::atoms::Numeric::sptr g = std::dynamic_pointer_cast<sight::atoms::Numeric>((*ambientSeq)[1]);
    g->setFromString(".05");
    sight::atoms::Numeric::sptr b = std::dynamic_pointer_cast<sight::atoms::Numeric>((*ambientSeq)[2]);
    b->setFromString(".05");
    sight::atoms::Numeric::sptr a = std::dynamic_pointer_cast<sight::atoms::Numeric>((*ambientSeq)[3]);
    a->setFromString("1.");
}

} // namespace Material

} // namespace data

} // namespace sight::io::patch::structural
