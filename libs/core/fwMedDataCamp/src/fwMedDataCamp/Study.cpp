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

#include <fwCamp/UserObject.hpp>

#include <fwMedData/Study.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(Study))
{
    builder
    .tag("object_version", "2")
    .tag("lib_name", "fwMedData")
    .base< ::fwData::Object>()
    .property("instance_uid", &::fwMedData::Study::m_instanceUID)
    .property("study_id", &::fwMedData::Study::m_studyID)
    .property("date", &::fwMedData::Study::m_date)
    .property("time", &::fwMedData::Study::m_time)
    .property("referring_physician_name", &::fwMedData::Study::m_referringPhysicianName)
    .property("consulting_physician_name", &::fwMedData::Study::m_consultingPhysicianName)
    .property("description", &::fwMedData::Study::m_description)
    .property("patient_age", &::fwMedData::Study::m_patientAge)
    .property("patient_size", &::fwMedData::Study::m_patientSize)
    .property("patient_weight", &::fwMedData::Study::m_patientWeight)
    .property("patient_body_mass_index", &::fwMedData::Study::m_patientBodyMassIndex)
    ;
}
