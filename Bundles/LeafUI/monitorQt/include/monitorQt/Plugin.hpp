/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MONITORQT_PLUGIN_HPP__
#define __MONITORQT_PLUGIN_HPP__


#include <fwRuntime/Plugin.hpp>


namespace monitor
{

/**
 * @date    2010.
 */
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

} // namespace monitor

#endif //__MONITORQT_PLUGIN_HPP__

