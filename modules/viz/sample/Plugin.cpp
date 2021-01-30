/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "modules/viz/sample/Plugin.hpp"

#include <core/runtime/operations.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace sight::modules::viz::sample
{

//------------------------------------------------------------------------------

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::sight::modules::viz::sample::Plugin");

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    // This module is intended to be used for the first tutorials.
    // Thus we load the adaptors model here instead in the .xml of the
    // application to hide the complexity.
    auto module = core::runtime::loadModule("module_viz_ogre");
    SLM_FATAL_IF("'visuOgreAdaptor' module failed to load.", !module);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

} // namespace sight::modules::viz::sample.
