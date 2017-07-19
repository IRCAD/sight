/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DEVFORUM_PLUGIN_HPP__
#define __DEVFORUM_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

/**
 * @brief Namespace devForum.
 * @namespace devForum
 *
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
    ~Plugin() noexcept;

    /// This method is used by runtime to initialize the bundle and register his services in OSR.
    void start();

    /// This method is used by runtime to stop the bundle, does nothing.
    void stop() noexcept;

};

} // namespace devForum

#endif //__DEVFORUM_PLUGIN_HPP__
