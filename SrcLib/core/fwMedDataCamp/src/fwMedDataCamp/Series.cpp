/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
