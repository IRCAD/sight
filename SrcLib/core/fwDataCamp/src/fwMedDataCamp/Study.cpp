/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMedData/Study.hpp>

#include <fwCamp/UserObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(Study))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwData::Object>()
    .property("instance_uid", &::fwMedData::Study::m_attrInstanceUID)
    .property("date", &::fwMedData::Study::m_attrDate)
    .property("time", &::fwMedData::Study::m_attrTime)
    .property("referring_physician_name", &::fwMedData::Study::m_attrReferringPhysicianName)
    .property("description", &::fwMedData::Study::m_attrDescription)
    .property("patient_age", &::fwMedData::Study::m_attrPatientAge)
    ;
}
