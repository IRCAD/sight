#ifndef __VIDEOVLC_PLUGIN_HPP__
#define __VIDEOVLC_PLUGIN_HPP__

#include "videoVLC/config.hpp"

#include <fwRuntime/Plugin.hpp>


namespace videoVLC
{
/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct VIDEOVLC_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VIDEOVLC_API void start();

    // Overrides
    VIDEOVLC_API void stop() noexcept;
};

} // namespace videoVLC

#endif //__VIDEOVLC_PLUGIN_HPP__
