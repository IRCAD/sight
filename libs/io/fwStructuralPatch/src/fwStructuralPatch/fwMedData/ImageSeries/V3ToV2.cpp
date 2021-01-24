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

#include "fwStructuralPatch/data/ImageSeries/V3ToV2.hpp"

#include <fwAtoms/String.hpp>

namespace fwStructuralPatch
{
namespace sight::data
{
namespace ImageSeries
{

V3ToV2::V3ToV2() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "data::ImageSeries";
    m_targetClassname = "data::ImageSeries";
    m_originVersion   = "3";
    m_targetVersion   = "2";
}

// ----------------------------------------------------------------------------

V3ToV2::~V3ToV2()
{
}

// ----------------------------------------------------------------------------

V3ToV2::V3ToV2(const V3ToV2& _cpy) :
    ::fwAtomsPatch::IStructuralPatch(_cpy)
{
}

// ----------------------------------------------------------------------------

void V3ToV2::apply( const ::fwAtoms::Object::sptr& _previous,
                    const ::fwAtoms::Object::sptr& _current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& _newVersions)
{
    IStructuralPatch::apply(_previous, _current, _newVersions);

    // Updates object version.
    this->updateVersion(_current);

    ::fwAtomsPatch::helper::Object helper(_current);
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

    helper.removeAttribute("contrast_bolus_agent");
    helper.removeAttribute("contrast_bolus_route");
    helper.removeAttribute("contrast_bolus_volume");
    helper.removeAttribute("contrast_bolus_start_time");
    helper.removeAttribute("contrast_bolus_stop_time");
    helper.removeAttribute("contrast_bolus_total_dose");
    helper.removeAttribute("contrast_bolus_flow_rate");
    helper.removeAttribute("contrast_bolus_flow_duration");
    helper.removeAttribute("contrast_bolus_ingredient");
    helper.removeAttribute("contrast_bolus_ingredient_concentration");
    helper.removeAttribute("acquisition_date");
    helper.removeAttribute("acquisition_time");
}

} // namespace ImageSeries
} // namespace sight::data
} // namespace fwStructuralPatch
