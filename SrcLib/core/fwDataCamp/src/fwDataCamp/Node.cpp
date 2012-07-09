#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Node.hpp"

fwCampImplementDataMacro((fwData)(Node))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("object", &::fwData::Node::m_object)
        .property("inputs", &::fwData::Node::m_inputs)
        .property("outputs", &::fwData::Node::m_outputs)
        ;
}

