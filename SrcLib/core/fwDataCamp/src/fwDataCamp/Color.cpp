#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Color.hpp"

fwCampImplementDataMacro((fwData)(Color))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("rgba", &::fwData::Color::m_vRGBA)
        ;
}

