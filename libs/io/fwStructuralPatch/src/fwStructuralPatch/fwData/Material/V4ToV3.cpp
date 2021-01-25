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

#include "fwStructuralPatch/data/Material/V4ToV3.hpp"

#include <atoms/Boolean.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

namespace fwStructuralPatch
{

namespace sight::data
{

namespace Material
{

V4ToV3::V4ToV3() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::sight::data::Material";
    m_targetClassname = "::sight::data::Material";
    m_originVersion   = "4";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V4ToV3::~V4ToV3()
{
}

// ----------------------------------------------------------------------------

V4ToV3::V4ToV3( const V4ToV3& cpy ) :
    ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V4ToV3::apply( const atoms::Object::sptr& previous,
                    const atoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    ::fwAtomsPatch::helper::Object helper(current);

    // Retrieve shading_mode and replace AMBIENT mode with PHONG
    // add lighting attribute (false if mode is AMBIENT, else true)
    atoms::String::sptr shadingMode = previous->getAttribute< atoms::String >("shading_mode");
    std::string shading             = shadingMode->getValue();

    bool lighting = true;
    if (shading == "AMBIENT")
    {
        lighting = false;
        helper.replaceAttribute("shading_mode", atoms::String::New("PHONG"));
    }
    helper.addAttribute("lighting", atoms::Boolean::New(lighting));

    // Switch diffuse and ambient
    atoms::Object::sptr ambient = current->getAttribute< atoms::Object >("ambient");
    atoms::Object::sptr diffuse = current->getAttribute< atoms::Object >("diffuse");

    // Replace diffuse by previous ambient
    helper.replaceAttribute("diffuse", ambient);
    helper.replaceAttribute("ambient", diffuse);

    // Replace options_mode "CELLS_NORMALS" by "NORMALS"
    atoms::String::sptr optionMode = previous->getAttribute< atoms::String >("options_mode");
    std::string option             = optionMode->getValue();
    if (option == "CELLS_NORMALS")
    {
        helper.replaceAttribute("options_mode", atoms::String::New("NORMALS"));
    }

}

} // namespace Material

} // namespace sight::data

} // namespace fwStructuralPatch
