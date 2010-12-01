#ifndef _OPSOFA_PLUGIN_HPP_
#define _OPSOFA_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>


namespace opSofa
{


/**
 * @brief Implements a plugin for the opSofa bundle.
 */
struct Plugin : public ::fwRuntime::Plugin
{
    ~Plugin() throw();
    void start() throw(::fwRuntime::RuntimeException);
    void stop() throw();
};


} // namespace opSofa


#endif //_OPSOFA_PLUGIN_HPP_
