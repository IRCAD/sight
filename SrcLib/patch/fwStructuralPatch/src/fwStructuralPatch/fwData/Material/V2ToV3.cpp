/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwStructuralPatch/fwData/Material/V2ToV3.hpp"

#include <fwAtoms/String.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>


namespace fwStructuralPatch
{

namespace fwData
{

namespace Material
{

V2ToV3::V2ToV3() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Material";
    m_targetClassname = "::fwData::Material";
    m_originVersion   = "2";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V2ToV3::~V2ToV3()
{
}

// ----------------------------------------------------------------------------

V2ToV3::V2ToV3( const V2ToV3 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V2ToV3::apply(
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);
    helper.addAttribute("lighting", ::fwAtoms::Boolean::New(true) );
}

} // namespace Material

} // namespace fwData

} // namespace fwStructuralPatch

