/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "activities/Plugin.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace activities
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("activities::Plugin");

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    ::fwActivities::registry::Activities::getDefault()->parseBundleInformation();
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    // Clear all operator configurations
    ::fwActivities::registry::Activities::getDefault()->clearRegistry();
}

} // namespace activities
