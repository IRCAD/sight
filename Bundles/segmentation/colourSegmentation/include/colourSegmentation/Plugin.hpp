/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __COLOURSEGMENTATION_PLUGIN_HPP__
#define __COLOURSEGMENTATION_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace colourSegmentation
{

/**
 * @brief   This class is started when the bundles is loaded.
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

} // namespace segmentation

#endif //__COLOURSEGMENTATION_PLUGIN_HPP__
