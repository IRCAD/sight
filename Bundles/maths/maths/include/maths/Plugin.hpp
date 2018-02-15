/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MATHS_PLUGIN_HPP__
#define __MATHS_PLUGIN_HPP__

#include "maths/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace maths
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct MATHS_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    MATHS_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    MATHS_API void start();

    /// Overrides stop method. Do nothing
    MATHS_API void stop() noexcept;

};

} // namespace maths

#endif //__MATHS_PLUGIN_HPP__
