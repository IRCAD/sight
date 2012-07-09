#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/location/SingleFile.hpp"

fwCampImplementDataMacro((fwData)(location)(SingleFile))
{
    builder.base< ::fwData::Object>()
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .property("path", &::fwData::location::SingleFile::m_path)
        ;
}
