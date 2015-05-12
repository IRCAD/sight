/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMedData/Patient.hpp>

#include <fwCamp/UserObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(Patient))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwData::Object>()
    .property("name", &::fwMedData::Patient::m_attrName)
    .property("patient_id", &::fwMedData::Patient::m_attrPatientId)
    .property("birth_date", &::fwMedData::Patient::m_attrBirthdate)
    .property("sex", &::fwMedData::Patient::m_attrSex)
    ;
}
