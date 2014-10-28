/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/StructureTraitsDictionary.hpp>

#include <fwCamp/UserObject.hpp>

fwCampImplementDataMacro((fwData)(StructureTraitsDictionary))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("structureTraits", &::fwData::StructureTraitsDictionary::m_structureTraitsMap)
        ;
}
