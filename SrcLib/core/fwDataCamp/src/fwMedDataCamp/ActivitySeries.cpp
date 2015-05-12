/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Composite.hpp>
#include <fwMedData/ActivitySeries.hpp>

#include <fwCamp/UserObject.hpp>

#include "fwMedDataCamp/ActivitySeries.hpp"

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(ActivitySeries))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwActivities")
    .base< ::fwMedData::Series>()
    .property("activity_config_id", &::fwMedData::ActivitySeries::m_attrActivityConfigId)
    .property("data", &::fwMedData::ActivitySeries::m_attrData)
    ;
}
