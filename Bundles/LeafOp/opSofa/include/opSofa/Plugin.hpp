/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPSOFA_PLUGIN_HPP__
#define __OPSOFA_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>


namespace opSofa
{


/**
 * @brief Implements a plugin for the opSofa bundle.
 */
struct Plugin : public ::fwRuntime::Plugin
{
    ~Plugin() noexcept;
    void start();
    void stop() noexcept;
};


} // namespace opSofa


#endif //__OPSOFA_PLUGIN_HPP__
