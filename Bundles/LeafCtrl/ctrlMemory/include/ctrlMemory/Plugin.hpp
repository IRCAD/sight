/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLMEMORY_PLUGIN_HPP_
#define _CTRLMEMORY_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>


namespace ctrlMemory
{

class Plugin : public ::fwRuntime::Plugin
{

public :

    /// PLugin destructor
    ~Plugin() throw();

    /// This method is used by runtime to initialize the bundle.
    void start() throw( ::fwRuntime::RuntimeException );

    /// This method is used by runtime to stop the bundle.
    void stop() throw();

};

} // namespace ctrlMemory

#endif // _CTRLMEMORY_PLUGIN_HPP_
