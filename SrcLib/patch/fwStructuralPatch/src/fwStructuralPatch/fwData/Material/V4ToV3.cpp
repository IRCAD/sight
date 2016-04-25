/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwStructuralPatch/fwData/Material/V4ToV3.hpp"

#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

namespace fwStructuralPatch
{

namespace fwData
{

namespace Material
{

V4ToV3::V4ToV3() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Material";
    m_targetClassname = "::fwData::Material";
    m_originVersion   = "4";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V4ToV3::~V4ToV3()
{
}

// ----------------------------------------------------------------------------

V4ToV3::V4ToV3( const V4ToV3 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V4ToV3::apply( const ::fwAtoms::Object::sptr& previous,
                    const ::fwAtoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    ::fwAtomsPatch::helper::Object helper(current);

    // Retrieve shading_mode and replace AMBIENT mode with PHONG
    // add lighting attribute (false if mode is AMBIENT, else true)
    ::fwAtoms::String::sptr shadingMode = previous->getAttribute< ::fwAtoms::String >("shading_mode");
    std::string shading = shadingMode->getValue();

    bool lighting = true;
    if (shading == "AMBIENT")
    {
        lighting = false;
        helper.replaceAttribute("shading_mode", ::fwAtoms::String::New("PHONG"));
    }
    helper.addAttribute("lighting", ::fwAtoms::Boolean::New(lighting));



    // Switch diffuse and ambient
    ::fwAtoms::Object::sptr ambient = current->getAttribute< ::fwAtoms::Object >("ambient");
    ::fwAtoms::Object::sptr diffuse = current->getAttribute< ::fwAtoms::Object >("diffuse");

    // Replace diffuse by previous ambient
    helper.replaceAttribute("diffuse", ambient);
    helper.replaceAttribute("ambient", diffuse);

    // Replace options_mode "CELLS_NORMALS" by "NORMALS"
    ::fwAtoms::String::sptr optionMode = previous->getAttribute< ::fwAtoms::String >("options_mode");
    std::string option = optionMode->getValue();
    if (option == "CELLS_NORMALS")
    {
        helper.replaceAttribute("options_mode", ::fwAtoms::String::New("NORMALS"));
    }

}

} // namespace Material

} // namespace fwData

} // namespace fwStructuralPatch
