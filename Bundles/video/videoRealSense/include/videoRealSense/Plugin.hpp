#ifndef __VIDEOREALSENSE_PLUGIN_HPP__
#define __VIDEOREALSENSE_PLUGIN_HPP__

#include "videoRealSense/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace videoRealSense
{
/**
 * @brief   This class is started when the bundles is loaded.
 */
struct VIDEOREALSENSE_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    VIDEOREALSENSE_API ~Plugin() noexcept;

    // Overrides
    VIDEOREALSENSE_API void start();

    // Overrides
    VIDEOREALSENSE_API void stop() noexcept;

};

} // namespace videoRealSense

#endif //__VIDEOREALSENSE_PLUGIN_HPP__
