#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Patient.hpp"

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
