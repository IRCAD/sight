/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DEVFORUM_PLUGIN_HPP_
#define _DEVFORUM_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

/**
 * @brief Namespace devForum.
 * @namespace devForum.
 * @author IRCAD (Research and Development Team).
 * @date 2009.
 *
 *  The namespace devForum contains tutorial to use framework.
 */
namespace devForum
{

class Plugin : public ::fwRuntime::Plugin
{

public:

    /// Destructor, does nothing.
    ~Plugin() throw();
    
    /// This method is used by runtime to initialize the bundle and register his services in OSR.
    void start() throw ( ::fwRuntime::RuntimeException );

    /// This method is used by runtime to stop the bundle, does nothing.
    void stop() throw();
    
};

} // namespace devForum

#endif //_DEVFORUM_PLUGIN_HPP_
