/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#define CAMP_COMPILATION

#include "data/detail/reflection/Series.hpp"
#include "data/Series.hpp"

#include "data/Equipment.hpp"
#include "data/Patient.hpp"
#include "data/Study.hpp"

#include <core/reflection/UserObject.hpp>

//------------------------------------------------------------------------------

SIGHT_IMPLEMENT_DATA_REFLECTION((sight) (data) (Series))
{
    builder
    .tag("object_version", "2")
    .tag("lib_name", "::sight::data")
    .base< ::sight::data::Object>()
    .property("patient", &::sight::data::Series::m_patient)
    .property("study", &::sight::data::Series::m_study)
    .property("equipment", &::sight::data::Series::m_equipment)
    .property("modality", &::sight::data::Series::m_modality)
    .property("instance_uid", &::sight::data::Series::m_instanceUID)
    .property("number", &::sight::data::Series::m_number)
    .property("laterality", &::sight::data::Series::m_laterality)
    .property("date", &::sight::data::Series::m_date)
    .property("time", &::sight::data::Series::m_time)
    .property("performing_physicians_name", &::sight::data::Series::m_performingPhysiciansName)
    .property("protocolName", &::sight::data::Series::m_protocolName)
    .property("description", &::sight::data::Series::m_description)
    .property("body_part_examined", &::sight::data::Series::m_bodyPartExamined)
    .property("patient_position", &::sight::data::Series::m_patientPosition)
    .property("anatomical_orientation_type", &::sight::data::Series::m_anatomicalOrientationType)
    .property("performded_procedure_step_id", &::sight::data::Series::m_performdedProcedureStepID)
    .property("performed_procedure_step_start_date", &::sight::data::Series::m_performedProcedureStepStartDate)
    .property("performed_procedure_step_start_time", &::sight::data::Series::m_performedProcedureStepStartTime)
    .property("performed_procedure_step_end_date", &::sight::data::Series::m_performedProcedureStepEndDate)
    .property("performed_procedure_step_end_time", &::sight::data::Series::m_performedProcedureStepEndTime)
    .property("performed_procedure_step_description", &::sight::data::Series::m_performedProcedureStepDescription)
    .property("performed_procedure_comments", &::sight::data::Series::m_performedProcedureComments)
    ;
}
