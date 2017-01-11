/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedDataCamp/camp/mapper.hpp"

#include <fwCamp/UserObject.hpp>

#include <fwMedData/NavigationSeries.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(NavigationSeries))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwMedData::Series>()
    .property("position", &::fwMedData::NavigationSeries::m_position)
    .property("move", &::fwMedData::NavigationSeries::m_move)
    .property("look_at", &::fwMedData::NavigationSeries::m_lookAt)
    ;
}
