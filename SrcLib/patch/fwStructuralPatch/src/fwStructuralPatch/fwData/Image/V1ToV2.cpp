/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/String.hpp>

#include "fwStructuralPatch/fwData/Image/V1ToV2.hpp"

namespace fwStructuralPatch
{

namespace fwData
{

namespace Image
{

V1ToV2::V1ToV2() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Image";
    m_targetClassname = "::fwData::Image";
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

    helper.addAttribute("nb_components", ::fwAtoms::Numeric::New(1));
    helper.addAttribute("window_center", ::fwAtoms::Numeric::New(50));
    helper.addAttribute("window_width", ::fwAtoms::Numeric::New(500));
}

} // namespace Image

} // namespace fwData

} // namespace fwStructuralPatch

