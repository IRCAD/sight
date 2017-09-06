/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_PLUGIN_HPP__
#define __CTRLSELECTION_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace ctrlSelection
{

class Plugin : public ::fwRuntime::Plugin
{

public:

    /// PLugin destructor
    ~Plugin() noexcept;

    /// This method is used by runtime to initialize the bundle.
    void start();

    /// This method is used by runtime to stop the bundle.
    void stop() noexcept;

};

} // namespace ctrlSelection

#endif // __CTRLSELECTION_PLUGIN_HPP__
