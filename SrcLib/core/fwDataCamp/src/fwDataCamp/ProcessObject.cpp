#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/ProcessObject.hpp"

fwCampImplementDataMacro((fwData)(ProcessObject))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object >()
        .property("inputs", &fwData::ProcessObject::m_attrInputs)
        .property("outputs", &fwData::ProcessObject::m_attrOutputs)
        ;
}
