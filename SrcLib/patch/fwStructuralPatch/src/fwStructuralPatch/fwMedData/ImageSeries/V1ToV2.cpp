/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwStructuralPatch/fwMedData/ImageSeries/V1ToV2.hpp"

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>

namespace fwStructuralPatch
{
namespace fwMedData
{
namespace ImageSeries
{

V1ToV2::V1ToV2() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwMedData::ImageSeries";
    m_targetClassname = "::fwMedData::ImageSeries";
    m_originVersion   = "1";
    m_targetVersion   = "2";
}

// ----------------------------------------------------------------------------

V1ToV2::~V1ToV2()
{
}

// ----------------------------------------------------------------------------

V1ToV2::V1ToV2( const V1ToV2& cpy ) :
    ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV2::apply( const ::fwAtoms::Object::sptr& previous,
                    const ::fwAtoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    ::fwAtomsPatch::helper::Object helper(current);
    helper.addAttribute("dicom_reference", ::fwAtoms::Object::sptr());
}

} // namespace ImageSeries
} // namespace fwMedData
} // namespace fwStructuralPatch
