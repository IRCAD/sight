/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Composite.hpp>
#include <fwActivities/ActivitySeries.hpp>

#include <fwCamp/UserObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwActivities)(ActivitySeries))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwActivities")
        .base< ::fwMedData::Series>()
        .property("activity_config_id", &::fwActivities::ActivitySeries::m_attrActivityConfigId)
        .property("data", &::fwActivities::ActivitySeries::m_attrData)
        ;
}
