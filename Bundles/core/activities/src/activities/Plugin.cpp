#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "activities/Plugin.hpp"
#include "fwMedData/ActivitySeries.hpp"
#include "fwActivities/registry/Activities.hpp"

namespace activities
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("activities::Plugin");

Plugin::~Plugin() throw()
{
    //Hack: force link with fwActivities
    ::fwMedData::ActivitySeries::NewSptr aSeries;
    aSeries->getClassname();
}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    ::fwActivities::registry::Activities::getDefault()->parseBundleInformation();
}

void Plugin::stop() throw()
{
    // Clear all operator configurations
    ::fwActivities::registry::Activities::getDefault()->clearRegistry();
}

} // namespace activities
