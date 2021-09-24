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

#include "io/patch/structural/data/ActivitySeries/V1ToV2.hpp"

#include <atoms/String.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace ActivitySeries
{

V1ToV2::V1ToV2() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "sight::data::ActivitySeries";
    m_targetClassname = "sight::data::ActivitySeries";
    m_originVersion   = "1";
    m_targetVersion   = "2";
}

// ----------------------------------------------------------------------------

V1ToV2::~V1ToV2()
{
}

// ----------------------------------------------------------------------------

V1ToV2::V1ToV2(const V1ToV2& _cpy) :
    io::atoms::patch::IStructuralPatch(_cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV2::apply(
    const sight::atoms::Object::sptr& _previous,
    const sight::atoms::Object::sptr& _current,
    io::atoms::patch::IPatch::NewVersionsType& _newVersions
)
{
    IStructuralPatch::apply(_previous, _current, _newVersions);

    // Updates object version.
    this->updateVersion(_current);

    io::atoms::patch::helper::Object helper(_current);
    // We add Series attributes since the patcher can't retrieve the mother class of ImageSeries.
    // The patcher can't call mother class patch.
    helper.addAttribute("number", sight::atoms::String::New(""));
    helper.addAttribute("laterality", sight::atoms::String::New(""));
    helper.addAttribute("protocolName", sight::atoms::String::New(""));
    helper.addAttribute("body_part_examined", sight::atoms::String::New(""));
    helper.addAttribute("patient_position", sight::atoms::String::New(""));
    helper.addAttribute("anatomical_orientation_type", sight::atoms::String::New(""));
    helper.addAttribute("performded_procedure_step_id", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_date", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_time", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_date", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_time", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_description", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_comments", sight::atoms::String::New(""));
}

} // namespace ActivitySeries

} // namespace data

} // namespace sight::io::patch::structural
