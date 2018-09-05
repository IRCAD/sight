/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREQT_PLUGIN_HPP__
#define __VISUOGREQT_PLUGIN_HPP__

#include "visuOgreQt/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace visuOgreQt
{
/**
 * @brief   This class is started when the bundles is loaded.
 */
struct VISUOGREQT_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VISUOGREQT_API void start();

    // Overrides
    VISUOGREQT_API void stop() noexcept;

};

} // namespace visuOgreQt

#endif //__VISUOGREQT_PLUGIN_HPP__
