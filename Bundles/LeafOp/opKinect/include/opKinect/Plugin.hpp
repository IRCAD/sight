#ifndef _OPKINECT_PLUGIN_HPP_
#define _OPKINECT_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>


namespace opKinect
{


/**
 * @brief Implements a plugin for the opKinect bundle.
 */
struct Plugin : public ::fwRuntime::Plugin
{
    ~Plugin() throw();
    void start() throw(::fwRuntime::RuntimeException);
    void stop() throw();
};


} // namespace opKinect


#endif //_OPKINECT_PLUGIN_HPP_
