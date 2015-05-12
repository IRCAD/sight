/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMedData/Series.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Equipment.hpp>

#include <fwCamp/UserObject.hpp>

#include "fwMedDataCamp/Series.hpp"

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(Series))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwData::Object>()
    .property("patient", &::fwMedData::Series::m_attrPatient)
    .property("study", &::fwMedData::Series::m_attrStudy)
    .property("equipment", &::fwMedData::Series::m_attrEquipment)
    .property("instance_uid", &::fwMedData::Series::m_attrInstanceUID)
    .property("modality", &::fwMedData::Series::m_attrModality)
    .property("date", &::fwMedData::Series::m_attrDate)
    .property("time", &::fwMedData::Series::m_attrTime)
    .property("performing_physicians_name", &::fwMedData::Series::m_attrPerformingPhysiciansName)
    .property("description", &::fwMedData::Series::m_attrDescription)
    ;
}
