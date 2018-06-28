#ifndef __VIDEOORBBEC_PLUGIN_HPP__
#define __VIDEOORBBEC_PLUGIN_HPP__

#include "videoOrbbec/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace videoOrbbec
{
/**
 * @brief   This class is started when the bundle is loaded.
 */
struct VIDEOORBBEC_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VIDEOORBBEC_API void start();

    // Overrides
    VIDEOORBBEC_API void stop() noexcept;

};

} // namespace videoOrbbec

#endif // __VIDEOORBBEC_PLUGIN_HPP__
