/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/String.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>

#include "fwStructuralPatch/fwData/Material/V1ToV2.hpp"

namespace fwStructuralPatch
{

namespace fwData
{

namespace Material
{

V1ToV2::V1ToV2() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Material";
    m_targetClassname = "::fwData::Material";
    m_originVersion = "1";
    m_targetVersion = "2";
}

// ----------------------------------------------------------------------------

V1ToV2::~V1ToV2()
{
}

// ----------------------------------------------------------------------------

V1ToV2::V1ToV2( const V1ToV2 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV2::apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);
    helper.addAttribute("diffuse_texture", ::fwAtoms::Object::sptr() );
    helper.addAttribute("diffuse_texture_filtering", ::fwAtoms::String::New("LINEAR"));
    helper.addAttribute("diffuse_texture_wrapping", ::fwAtoms::String::New("REPEAT"));
}

} // namespace Material

} // namespace fwData

} // namespace fwStructuralPatch

