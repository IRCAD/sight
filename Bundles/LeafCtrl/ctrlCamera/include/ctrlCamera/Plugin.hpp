#ifndef __CTRLCAMERA_PLUGIN_HPP__
#define __CTRLCAMERA_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "ctrlCamera/config.hpp"

namespace ctrlCamera
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct CTRLCAMERA_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    CTRLCAMERA_API ~Plugin() throw();

    /**
     * @brief Overrides start method. Object declarations.
     */
    CTRLCAMERA_API void start() throw( ::fwRuntime::RuntimeException );

    /// Overrides stop method. Do nothing
    CTRLCAMERA_API void stop() throw();

};

} // namespace ctrlCamera

#endif //__CTRLCAMERA_PLUGIN_HPP__
