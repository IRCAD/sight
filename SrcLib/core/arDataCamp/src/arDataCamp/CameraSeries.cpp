/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>

#include <arData/CameraSeries.hpp>

#include "arDataCamp/CameraSeries.hpp"

//------------------------------------------------------------------------------

fwCampImplementDataMacro((arData)(CameraSeries))
{
    builder
    .tag("object_version", "2")
    .tag("lib_name", "arData")
    .base< ::fwMedData::Series>()
    .property("cameras", &::arData::CameraSeries::m_cameras)
    .property("extrinsic_matrices", &::arData::CameraSeries::m_extrinsicMatrices)
    ;
}
