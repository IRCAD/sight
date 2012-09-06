
#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Acquisition.hpp"

fwCampImplementDataMacro((fwData)(Object))
{
    builder
        .property("fields", &::fwData::Object::m_fields)
        .function("classname", (std::string (fwData::Object::*)() const) &::fwData::Object::className)
        ;
}
