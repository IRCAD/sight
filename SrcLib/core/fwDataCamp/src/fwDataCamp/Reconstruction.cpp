/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>
#include "fwDataCamp/Reconstruction.hpp"

fwCampImplementDataMacro((fwData)(Reconstruction))
{
    builder
        .tag("object_version", "3")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("is_visible", &::fwData::Reconstruction::m_bIsVisible)
        .property("organ_name", &::fwData::Reconstruction::m_sOrganName)
        .property("structure_type", &::fwData::Reconstruction::m_sStructureType)
        .property("material", &::fwData::Reconstruction::m_attrMaterial)
        .property("image", &::fwData::Reconstruction::m_attrImage)
        .property("mesh", &::fwData::Reconstruction::m_attrMesh)
        .property("volume", &::fwData::Reconstruction::m_attrComputedMaskVolume)
        ;
}
