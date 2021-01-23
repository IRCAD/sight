/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "memory/Plugin.hpp"

#include <core/base.hpp>
#include <core/memory/BufferManager.hpp>
#include <core/memory/policy/AlwaysDump.hpp>
#include <core/memory/policy/BarrierDump.hpp>
#include <core/memory/policy/NeverDump.hpp>
#include <core/memory/policy/ValveDump.hpp>
#include <core/runtime/helper.hpp>
#include <core/runtime/operations.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <core/tools/System.hpp>

namespace memory
{

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("memory::Plugin");

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    core::memory::BufferManager::sptr manager = core::memory::BufferManager::getDefault();

    const std::string modeKey = "loading_mode";

    if ( this->getModule()->hasParameter(modeKey) )
    {
        core::mt::WriteLock lock( manager->getMutex() );
        std::string mode = this->getModule()->getParameterValue(modeKey);
        if (mode == "lazy")
        {
            manager->setLoadingMode(core::memory::BufferManager::LAZY);
            SLM_INFO("Enabled lazy loading mode");
        }
        else if (mode == "direct")
        {
            manager->setLoadingMode(core::memory::BufferManager::DIRECT);
            SLM_INFO("Enabled direct loading mode");
        }
        else
        {
            SLM_ERROR("Unknown loading mode : '" + mode + "'");
        }
    }

}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

} // namespace memory
