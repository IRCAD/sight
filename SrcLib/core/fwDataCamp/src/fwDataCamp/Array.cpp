


#include <fwTools/camp/mapper.hpp>
#include "fwDataCamp/Array.hpp"
#include "fwDataCamp/Factory.hpp"

//------------------------------------------------------------------------------


fwCampImplementDataMacro((fwData)(Array))
{
    builder.base< ::fwData::Object>()
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .property("strides", &::fwData::Array::m_strides)
        .property("size", &::fwData::Array::m_size)
        .property("type", &::fwData::Array::m_type)
        .property("nb_of_components", &::fwData::Array::m_nbOfComponents)
        .property("buffer", &::fwData::Array::m_attrBufferObject)
        .property("isOwner", &::fwData::Array::m_isBufferOwner)
        ;
}
