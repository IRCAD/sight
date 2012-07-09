
#include "fwDataCamp/Factory.hpp"
#include "fwData/camp/mapper.hpp"
#include "fwDataCamp/PatientDB.hpp"

fwCampImplementDataMacro((fwData)(PatientDB))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("patients", &::fwData::PatientDB::m_attrPatients)
        ;
}
