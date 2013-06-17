/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/String.hpp>

#include "fwStructuralPatch/fwData/Study/V1ToFwMedDataStudyV1.hpp"

namespace fwStructuralPatch
{

namespace fwData
{

namespace Study
{

V1ToFwMedDataStudyV1::V1ToFwMedDataStudyV1() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Study";
    m_targetClassname = "::fwMedData::Study";
    m_originVersion = "1";
    m_targetVersion = "1";
}

// ----------------------------------------------------------------------------

V1ToFwMedDataStudyV1::~V1ToFwMedDataStudyV1()
{
}

// ----------------------------------------------------------------------------

V1ToFwMedDataStudyV1::V1ToFwMedDataStudyV1( const V1ToFwMedDataStudyV1 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToFwMedDataStudyV1::apply(const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    helper.removeAttribute("hospital");
    helper.removeAttribute("modality");
    helper.removeAttribute("acquisition_zone");
    helper.removeAttribute("ris_id");
    helper.removeAttribute("db_id");
    helper.removeAttribute("acquisitions");

    helper.renameAttribute("uid", "instance_uid");

    helper.addAttribute("referring_physician_name", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_age", ::fwAtoms::String::New(""));
}

} // namespace Study

} // namespace fwData

} // namespace fwStructuralPatch


