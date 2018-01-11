/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUQT_PLUGIN_HPP__
#define __UIVISUQT_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace uiVisuQt
{
/**
 */
struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    void start();

    // Overrides
    void stop() noexcept;

};

} // namespace uiVisuQt

#endif //__UIVISUQT_PLUGIN_HPP__
