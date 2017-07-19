/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwMedData/ActivitySeries.hpp>
#include <fwActivities/registry/Activities.hpp>

#include "activities/Plugin.hpp"

namespace activities
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("activities::Plugin");

Plugin::~Plugin() noexcept
{
    //Hack: force link with fwActivities
    ::fwMedData::ActivitySeries::sptr aSeries = ::fwMedData::ActivitySeries::New();
    aSeries->getClassname();
}

void Plugin::start()
{
    ::fwActivities::registry::Activities::getDefault()->parseBundleInformation();
}

void Plugin::stop() noexcept
{
    // Clear all operator configurations
    ::fwActivities::registry::Activities::getDefault()->clearRegistry();
}

} // namespace activities
