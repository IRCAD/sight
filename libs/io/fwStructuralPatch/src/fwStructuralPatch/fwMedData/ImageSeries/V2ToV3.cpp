/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "fwStructuralPatch/data/ImageSeries/V2ToV3.hpp"

#include <atoms/String.hpp>

namespace fwStructuralPatch
{
namespace sight::data
{
namespace ImageSeries
{

V2ToV3::V2ToV3() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::sight::data::ImageSeries";
    m_targetClassname = "::sight::data::ImageSeries";
    m_originVersion   = "2";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V2ToV3::~V2ToV3()
{
}

// ----------------------------------------------------------------------------

V2ToV3::V2ToV3(const V2ToV3& _cpy) :
    ::fwAtomsPatch::IStructuralPatch(_cpy)
{
}

// ----------------------------------------------------------------------------

void V2ToV3::apply( const atoms::Object::sptr& _previous,
                    const atoms::Object::sptr& _current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& _newVersions)
{
    IStructuralPatch::apply(_previous, _current, _newVersions);

    // Updates object version.
    this->updateVersion(_current);

    ::fwAtomsPatch::helper::Object helper(_current);
    // We add Series attributes since the patcher can't retrieve the mother class of ImageSeries.
    // The patcher can't call mother class patch.
    helper.addAttribute("number", atoms::String::New(""));
    helper.addAttribute("laterality", atoms::String::New(""));
    helper.addAttribute("protocolName", atoms::String::New(""));
    helper.addAttribute("body_part_examined", atoms::String::New(""));
    helper.addAttribute("patient_position", atoms::String::New(""));
    helper.addAttribute("anatomical_orientation_type", atoms::String::New(""));
    helper.addAttribute("performded_procedure_step_id", atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_date", atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_time", atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_date", atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_time", atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_description", atoms::String::New(""));
    helper.addAttribute("performed_procedure_comments", atoms::String::New(""));

    helper.addAttribute("contrast_bolus_agent", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_route", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_volume", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_start_time", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_stop_time", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_total_dose", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_flow_rate", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_flow_duration", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_ingredient", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_ingredient_concentration", atoms::String::New(""));
    helper.addAttribute("acquisition_date", atoms::String::New(""));
    helper.addAttribute("acquisition_time", atoms::String::New(""));
}

} // namespace ImageSeries
} // namespace sight::data
} // namespace fwStructuralPatch
