#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Model.hpp"

fwCampImplementDataMacro((fwData)(Model))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        // .property("map", &::fwData::Model::m_map)
        ;
}
