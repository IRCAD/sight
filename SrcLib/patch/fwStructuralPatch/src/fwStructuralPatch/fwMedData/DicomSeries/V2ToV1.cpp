/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwStructuralPatch/fwMedData/DicomSeries/V2ToV1.hpp"

#include <fwAtoms/String.hpp>

namespace fwStructuralPatch
{
namespace fwMedData
{
namespace DicomSeries
{

V2ToV1::V2ToV1() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwMedData::DicomSeries";
    m_targetClassname = "::fwMedData::DicomSeries";
    m_originVersion   = "2";
    m_targetVersion   = "1";
}

// ----------------------------------------------------------------------------

V2ToV1::~V2ToV1()
{
}

// ----------------------------------------------------------------------------

V2ToV1::V2ToV1(const V2ToV1& _cpy) :
    ::fwAtomsPatch::IStructuralPatch(_cpy)
{
}

// ----------------------------------------------------------------------------

void V2ToV1::apply( const ::fwAtoms::Object::sptr& previous,
                    const ::fwAtoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Updates object version.
    this->updateVersion(current);

    ::fwAtomsPatch::helper::Object helper(current);
    // We removes Series attributes since the patcher can't retrieve the mother class of ImageSeries.
    // The patcher can't call mother class patch.
    helper.removeAttribute("number");
    helper.removeAttribute("laterality");
    helper.removeAttribute("protocolName");
    helper.removeAttribute("body_part_examined");
    helper.removeAttribute("patient_position");
    helper.removeAttribute("anatomical_orientation_type");
    helper.removeAttribute("performded_procedure_step_id");
    helper.removeAttribute("performed_procedure_step_start_date");
    helper.removeAttribute("performed_procedure_step_start_time");
    helper.removeAttribute("performed_procedure_step_end_date");
    helper.removeAttribute("performed_procedure_step_end_time");
    helper.removeAttribute("performed_procedure_step_description");
    helper.removeAttribute("performed_procedure_comments");
}

} // namespace DicomSeries
} // namespace fwMedData
} // namespace fwStructuralPatch
