/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPKINECT_PLUGIN_HPP__
#define __OPKINECT_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>


namespace opKinect
{

/**
 * @brief Implements a plugin for the opKinect bundle.
 */
struct Plugin : public ::fwRuntime::Plugin
{
    ~Plugin() throw();
    /**
     * @brief start plugin
     */
    void start() throw(::fwRuntime::RuntimeException);
    /**
     * @brief stop plugin
     */
    void stop() throw();
};


} // namespace opKinect


#endif //__OPKINECT_PLUGIN_HPP__
