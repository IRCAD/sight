#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Histogram.hpp"

fwCampImplementDataMacro((fwData)(Histogram))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .property("value",&::fwData::Histogram::m_values)
        .property("bins_width",&::fwData::Histogram::m_binsWidth)
        .property("min_value",&::fwData::Histogram::m_minValue)
        .property("max_value",&::fwData::Histogram::m_maxValue)
        ;
}
