/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwRuntime/operations.hpp>
#include <fwRuntime/helper.hpp>

#include <fwCore/base.hpp>
#include <fwTools/System.hpp>

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/policy/AlwaysDump.hpp>
#include <fwMemory/policy/BarrierDump.hpp>
#include <fwMemory/policy/NeverDump.hpp>
#include <fwMemory/policy/ValveDump.hpp>

#include "memory/Plugin.hpp"

namespace memory
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("memory::Plugin");

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    ::fwMemory::BufferManager::sptr manager = ::fwMemory::BufferManager::getDefault();

    const std::string modeKey = "loading_mode";

    if ( this->getBundle()->hasParameter(modeKey) )
    {
        ::fwCore::mt::WriteLock lock( manager->getMutex() );
        std::string mode = this->getBundle()->getParameterValue(modeKey);
        if (mode == "lazy")
        {
            manager->setLoadingMode(::fwMemory::BufferManager::LAZY);
            SLM_INFO("Enabled lazy loading mode");
        }
        else if (mode == "direct")
        {
            manager->setLoadingMode(::fwMemory::BufferManager::DIRECT);
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
