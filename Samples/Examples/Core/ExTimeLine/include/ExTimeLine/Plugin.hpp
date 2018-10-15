/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ExTimeLine/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace ExTimeLine
{

/**
 * @brief Class call when bundle is started/stopped
 *
 */
class EXTIMELINE_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// PLugin destructor
    EXTIMELINE_API ~Plugin() noexcept;

    /// This method is used by runtime to initialize the bundle.
    EXTIMELINE_API void start();

    /// This method is used by runtime to stop the bundle.
    EXTIMELINE_API void stop() noexcept;

};

} // namespace ExTimeLine
