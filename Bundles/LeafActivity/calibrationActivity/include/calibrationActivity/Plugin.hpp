#ifndef __CALIBRATIONACTIVITY_PLUGIN_HPP__
#define __CALIBRATIONACTIVITY_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include <calibrationActivity/config.hpp>

namespace calibrationActivity
{

struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   Destructor
     */
    ~Plugin() throw();

    /**
     * @brief Start method.
     *
     * @exception ::fwRuntime::RuntimeException.
     * This method is used by runtime in order to initialize the bundle.
     */
    void start() throw(::fwRuntime::RuntimeException);

    /**
     * @brief Stop method.
     *
     * This method is used by runtime in order to close the bundle.
     */
    void stop() throw();
};

} // namespace calibrationActivity

#endif // __CALIBRATIONACTIVITY_PLUGIN_HPP__
