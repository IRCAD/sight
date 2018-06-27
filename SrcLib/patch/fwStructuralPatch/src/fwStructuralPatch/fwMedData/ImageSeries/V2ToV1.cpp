/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwStructuralPatch/fwMedData/ImageSeries/V2ToV1.hpp"

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>

namespace fwStructuralPatch
{
namespace fwMedData
{
namespace ImageSeries
{

V2ToV1::V2ToV1() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwMedData::ImageSeries";
    m_targetClassname = "::fwMedData::ImageSeries";
    m_originVersion   = "2";
    m_targetVersion   = "1";
}

// ----------------------------------------------------------------------------

V2ToV1::~V2ToV1()
{
}

// ----------------------------------------------------------------------------

V2ToV1::V2ToV1( const V2ToV1& cpy ) :
    ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V2ToV1::apply( const ::fwAtoms::Object::sptr& previous,
                    const ::fwAtoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    ::fwAtomsPatch::helper::Object helper(current);
    helper.removeAttribute("dicom_reference");
}

} // namespace ImageSeries
} // namespace fwMedData
} // namespace fwStructuralPatch
