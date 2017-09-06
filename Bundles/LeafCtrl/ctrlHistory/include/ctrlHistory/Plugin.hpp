/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLHISTORY_PLUGIN_HPP__
#define __CTRLHISTORY_PLUGIN_HPP__

#include "ctrlHistory/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace ctrlHistory
{

/**
 * @brief This class starts when the bundle is loaded.
 */
struct CTRLHISTORY_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    CTRLHISTORY_API ~Plugin() noexcept;

    /// Overrides start method. Does nothing.
    CTRLHISTORY_API void start();

    /// Overrides stop method. Does nothing.
    CTRLHISTORY_API void stop() noexcept;

};

} // namespace ctrlHistory

#endif //__CTRLHISTORY_PLUGIN_HPP__
