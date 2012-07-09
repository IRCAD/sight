#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Line.hpp"


fwCampImplementDataMacro((fwData)(Line))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .property("position", &::fwData::Line::m_position)
        .property("direction", &::fwData::Line::m_direction)
        ;
}
