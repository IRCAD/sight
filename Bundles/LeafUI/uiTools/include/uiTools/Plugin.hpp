/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UITOOLS_PLUGIN_HPP__
#define __UITOOLS_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "uiTools/config.hpp"

namespace uiTools
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct UITOOLS_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    UITOOLS_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    UITOOLS_API void start();

    /// Overrides stop method. Do nothing
    UITOOLS_API void stop() noexcept;

};

} // namespace uiTools

#endif //__UITOOLS_PLUGIN_HPP__
