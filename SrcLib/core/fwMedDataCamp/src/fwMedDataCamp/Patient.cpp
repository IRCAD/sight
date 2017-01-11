/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>

#include <fwMedData/Patient.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(Patient))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwData::Object>()
    .property("name", &::fwMedData::Patient::m_name)
    .property("patient_id", &::fwMedData::Patient::m_patientId)
    .property("birth_date", &::fwMedData::Patient::m_birthdate)
    .property("sex", &::fwMedData::Patient::m_sex)
    ;
}
