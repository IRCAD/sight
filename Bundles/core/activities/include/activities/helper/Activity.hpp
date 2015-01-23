/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ACTIVITIES_HELPER_ACTIVITY_HPP__
#define __ACTIVITIES_HELPER_ACTIVITY_HPP__

#include "activities/config.hpp"

#include <fwActivities/registry/Activities.hpp>
#include <fwMedData/ActivitySeries.hpp>

namespace fwServices
{
class ObjectMsg;
}

namespace activities
{

class ActivitySerie;

namespace helper
{

typedef ::fwActivities::registry::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;

    ACTIVITIES_API SPTR(::fwServices::ObjectMsg) buildActivityMsg(
            SPTR(::fwMedData::ActivitySeries) series,
            const ::fwActivities::registry::ActivityInfo& info,
            const ParametersType& parameters = ParametersType());

} // namespace helper
} // namespace activities

#endif // __ACTIVITIES_HELPER_ACTIVITY_HPP__
