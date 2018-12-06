/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwData::Object>()
    .property("patient", &::fwMedData::Series::m_patient)
    .property("study", &::fwMedData::Series::m_study)
    .property("equipment", &::fwMedData::Series::m_equipment)
    .property("instance_uid", &::fwMedData::Series::m_instanceUID)
    .property("modality", &::fwMedData::Series::m_modality)
    .property("date", &::fwMedData::Series::m_date)
    .property("time", &::fwMedData::Series::m_time)
    .property("performing_physicians_name", &::fwMedData::Series::m_performingPhysiciansName)
    .property("description", &::fwMedData::Series::m_description)
    ;
}
