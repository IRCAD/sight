/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Patient.hpp>

#include <fwCamp/UserObject.hpp>

fwCampImplementDataMacro((fwData)(Patient))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("name", &::fwData::Patient::m_sName)
        .property("firstname", &::fwData::Patient::m_sFirstname)
        .property("id_dicom", &::fwData::Patient::m_sIDDicom)
        .property("birthdate", &::fwData::Patient::m_sBirthdate)
        .property("is_male", &::fwData::Patient::m_bIsMale)
        .property("db_id", &::fwData::Patient::m_i32DbID)
        .property("studies", &::fwData::Patient::m_attrStudies)
        ;
}
