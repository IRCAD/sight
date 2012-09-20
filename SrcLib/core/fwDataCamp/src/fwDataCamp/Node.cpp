/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>
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

