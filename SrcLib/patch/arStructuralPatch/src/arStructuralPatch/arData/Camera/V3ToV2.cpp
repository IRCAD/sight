/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arStructuralPatch/arData/Camera/V3ToV2.hpp"

#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

namespace arStructuralPatch
{

namespace arData
{

namespace Camera
{

V3ToV2::V3ToV2() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::arData::Camera";
    m_targetClassname = "::arData::Camera";
    m_originVersion   = "3";
    m_targetVersion   = "2";

}

// ----------------------------------------------------------------------------

V3ToV2::~V3ToV2()
{
}

// ----------------------------------------------------------------------------

V3ToV2::V3ToV2( const V3ToV2& cpy ) :
    ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V3ToV2::apply(
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    helper.removeAttribute("scale");
}

} // namespace Camera

} // namespace arData

} // namespace arStructuralPatch

