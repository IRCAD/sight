/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwMedDataCamp/Series.hpp"

#include <fwCamp/UserObject.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(Series))
{
    builder
    .tag("object_version", "2")
    .tag("lib_name", "fwMedData")
    .base< ::fwData::Object>()
    .property("patient", &::fwMedData::Series::m_patient)
    .property("study", &::fwMedData::Series::m_study)
    .property("equipment", &::fwMedData::Series::m_equipment)
    .property("modality", &::fwMedData::Series::m_modality)
    .property("instance_uid", &::fwMedData::Series::m_instanceUID)
    .property("number", &::fwMedData::Series::m_number)
    .property("laterality", &::fwMedData::Series::m_laterality)
    .property("date", &::fwMedData::Series::m_date)
    .property("time", &::fwMedData::Series::m_time)
    .property("performing_physicians_name", &::fwMedData::Series::m_performingPhysiciansName)
    .property("protocolName", &::fwMedData::Series::m_protocolName)
    .property("description", &::fwMedData::Series::m_description)
    .property("body_part_examined", &::fwMedData::Series::m_bodyPartExamined)
    .property("patient_position", &::fwMedData::Series::m_patientPosition)
    .property("anatomical_orientation_type", &::fwMedData::Series::m_anatomicalOrientationType)
    .property("performded_procedure_step_id", &::fwMedData::Series::m_performdedProcedureStepID)
    .property("performed_procedure_step_start_date", &::fwMedData::Series::m_performedProcedureStepStartDate)
    .property("performed_procedure_step_start_time", &::fwMedData::Series::m_performedProcedureStepStartTime)
    .property("performed_procedure_step_end_date", &::fwMedData::Series::m_performedProcedureStepEndDate)
    .property("performed_procedure_step_end_time", &::fwMedData::Series::m_performedProcedureStepEndTime)
    .property("performed_procedure_step_description", &::fwMedData::Series::m_performedProcedureStepDescription)
    .property("performed_procedure_comments", &::fwMedData::Series::m_performedProcedureComments)
    ;
}
