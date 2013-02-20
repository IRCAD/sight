#ifndef _ACTIVITIES_PLUGIN_HPP_
#define _ACTIVITIES_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>
#include "activities/config.hpp"

namespace activities
{
/**
 * @author  IRCAD (Research and Development Team).
 * @date    2012.
 */
struct ACTIVITIES_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ACTIVITIES_API ~Plugin() throw();

    // Overrides
    ACTIVITIES_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    ACTIVITIES_API void stop() throw();

};

} // namespace activities

#endif //_ACTIVITIES_PLUGIN_HPP_
