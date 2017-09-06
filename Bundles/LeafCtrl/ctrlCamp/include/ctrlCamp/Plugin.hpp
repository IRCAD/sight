/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCAMP_PLUGIN_HPP__
#define __CTRLCAMP_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "ctrlCamp/config.hpp"

namespace ctrlCamp
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
struct CTRLCAMP_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    CTRLCAMP_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     */
    CTRLCAMP_API void start();

    /// Overrides stop method. Do nothing
    CTRLCAMP_API void stop() noexcept;

};

} // namespace ctrlCamp

#endif //__CTRLCAMP_PLUGIN_HPP__
