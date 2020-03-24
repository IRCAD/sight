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

#include "fwStructuralPatch/fwMedData/Study/V1ToV2.hpp"

#include <fwAtoms/String.hpp>

namespace fwStructuralPatch
{
namespace fwMedData
{
namespace Study
{

V1ToV2::V1ToV2() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwMedData::Study";
    m_targetClassname = "::fwMedData::Study";
    m_originVersion   = "1";
    m_targetVersion   = "2";
}

// ----------------------------------------------------------------------------

V1ToV2::~V1ToV2()
{
}

// ----------------------------------------------------------------------------

V1ToV2::V1ToV2(const V1ToV2& _cpy) :
    ::fwAtomsPatch::IStructuralPatch(_cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV2::apply( const ::fwAtoms::Object::sptr& _previous,
                    const ::fwAtoms::Object::sptr& _current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& _newVersions)
{
    IStructuralPatch::apply(_previous, _current, _newVersions);

    // Updates object version.
    this->updateVersion(_current);

    ::fwAtomsPatch::helper::Object helper(_current);
    // We add Series attributes since the patcher can't retrieve the mother class of ImageSeries.
    // The patcher can't call mother class patch.
    helper.addAttribute("number", ::fwAtoms::String::New(""));
    helper.addAttribute("laterality", ::fwAtoms::String::New(""));
    helper.addAttribute("protocolName", ::fwAtoms::String::New(""));
    helper.addAttribute("body_part_examined", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_position", ::fwAtoms::String::New(""));
    helper.addAttribute("anatomical_orientation_type", ::fwAtoms::String::New(""));
    helper.addAttribute("performded_procedure_step_id", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_date", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_time", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_date", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_time", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_description", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_comments", ::fwAtoms::String::New(""));

    helper.addAttribute("study_id", ::fwAtoms::String::New(""));
    helper.addAttribute("consulting_physician_name", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_size", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_weight", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_body_mass_index", ::fwAtoms::String::New(""));
}

} // namespace Study
} // namespace fwMedData
} // namespace fwStructuralPatch
