/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPVTKSLICER_PLUGIN_HPP__
#define __OPVTKSLICER_PLUGIN_HPP__

#include "opVTKSlicer/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace opVTKSlicer
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
struct OPVTKSLICER_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    OPVTKSLICER_API ~Plugin() noexcept;

    /// Overrides start method.
    OPVTKSLICER_API void start();

    /// Overrides stop method. Do nothing
    OPVTKSLICER_API void stop() noexcept;

};

} // namespace opVTKSlicer

#endif //__OPVTKSLICER_PLUGIN_HPP__
