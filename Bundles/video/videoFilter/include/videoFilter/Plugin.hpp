#ifndef __VIDEOFILTER_PLUGIN_HPP__
#define __VIDEOFILTER_PLUGIN_HPP__

#include "videoFilter/config.hpp"

#include <fwRuntime/Plugin.hpp>


namespace videoFilter
{
/**
 * @brief   This class is started when the bundles is loaded.
 */
struct VIDEOFILTER_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VIDEOFILTER_API void start();

    // Overrides
    VIDEOFILTER_API void stop() noexcept;
};

} // namespace videoFilter

#endif //__VIDEOFILTER_PLUGIN_HPP__
