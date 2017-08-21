/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIIMAGEQT_PLUGIN_HPP__
#define __UIIMAGEQT_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace uiImageQt
{
/**
 * @brief   Contains editors attached to image.
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

} // namespace uiImageQt

#endif //__UIIMAGEQT_PLUGIN_HPP__
