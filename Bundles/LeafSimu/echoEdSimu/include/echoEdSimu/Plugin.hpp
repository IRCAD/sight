/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ECHOEDSIMU_PLUGIN_HPP__
#define __ECHOEDSIMU_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "echoEdSimu/config.hpp"

namespace echoEdSimu
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct ECHOEDSIMU_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    ECHOEDSIMU_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    ECHOEDSIMU_API void start();

    /// Overrides stop method. Do nothing
    ECHOEDSIMU_API void stop() noexcept;

};

} // namespace echoEdSimu

#endif //__ECHOEDSIMU_PLUGIN_HPP__
