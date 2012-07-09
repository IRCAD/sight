#include "fwDataCamp/Factory.hpp"
#include <fwData/camp/mapper.hpp>
#include "fwDataCamp/location/Folder.hpp"


fwCampImplementDataMacro((fwData)(location)(Folder))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("folder", &::fwData::location::Folder::m_folder)
        .property("recursive", &::fwData::location::Folder::m_isRecursive)
        ;
}
