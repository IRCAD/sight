/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Study.hpp>

#include <fwCamp/UserObject.hpp>

fwCampImplementDataMacro((fwData)(Study))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("hospital", &::fwData::Study::m_sHospital)
        .property("modality", &::fwData::Study::m_sModality)
        .property("acquisition_zone", &::fwData::Study::m_sAcquisitionZone)
        .property("ris_id", &::fwData::Study::m_sRISId)
        .property("uid", &::fwData::Study::m_sUID)
        .property("db_id", &::fwData::Study::m_i32DbID)
        .property("date", &::fwData::Study::m_date)
        .property("time", &::fwData::Study::m_time)
        .property("description", &::fwData::Study::m_description)
        .property("acquisitions", &::fwData::Study::m_attrAcquisitions);

}
