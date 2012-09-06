#include "fwMetaData/Base.hpp"

fwCampImplementMacro((fwMetaData)(Base))
{
    builder
        .function("classname", &::fwMetaData::Base::className);
}
