/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __UICPR_PLUGIN_HPP__
#define __UICPR_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "uiCPR/config.hpp"

namespace uiCPR
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct UICPR_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    UICPR_API ~Plugin() noexcept;

    /// Overrides
    UICPR_API void start();

    /// Overrides
    UICPR_API void stop() noexcept;

};


} // namespace uiCPR


#endif //__UICPR_PLUGIN_HPP__
