#ifndef _IOXML_PLUGIN_HPP_
#define _IOXML_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

namespace ioXML
{

struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() throw();

    // Overrides
    void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    void stop() throw();

};

} // namespace ioXML

#endif //_IOXML_PLUGIN_HPP_
