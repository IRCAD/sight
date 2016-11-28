/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>

#include <fwMedData/Study.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(Study))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwData::Object>()
    .property("instance_uid", &::fwMedData::Study::m_instanceUID)
    .property("date", &::fwMedData::Study::m_date)
    .property("time", &::fwMedData::Study::m_time)
    .property("referring_physician_name", &::fwMedData::Study::m_referringPhysicianName)
    .property("description", &::fwMedData::Study::m_description)
    .property("patient_age", &::fwMedData::Study::m_patientAge)
    ;
}
