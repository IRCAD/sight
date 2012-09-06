/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
