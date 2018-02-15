/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "Tuto14MeshGenerator/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace Tuto14MeshGenerator
{

/**
 * @brief Class call when bundle is started/stopped
 */
class TUTO14MESHGENERATOR_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// PLugin destructor
    TUTO14MESHGENERATOR_API ~Plugin() noexcept;

    /// This method is used by runtime to initialize the bundle.
    TUTO14MESHGENERATOR_API void start();

    /// This method is used by runtime to stop the bundle.
    TUTO14MESHGENERATOR_API void stop() noexcept;
};

} // namespace Tuto14MeshGenerator

