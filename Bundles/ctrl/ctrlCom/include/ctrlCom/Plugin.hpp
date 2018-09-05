/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ctrlCom/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace ctrlCom
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
class CTRLCOM_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Destructor. Do nothing.
    CTRLCOM_API ~Plugin() noexcept;

    /// Do nothing.
    CTRLCOM_API void start();

    /// Do nothing
    CTRLCOM_API void stop() noexcept;
};

} // namespace ctrlCom
