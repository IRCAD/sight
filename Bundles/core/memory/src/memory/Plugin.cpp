/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwRuntime/operations.hpp>
#include <fwRuntime/helper.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>
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

Plugin::~Plugin() throw()
{}

//------------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    ::fwMemory::BufferManager::sptr manager = ::fwMemory::BufferManager::New();
    // manager->setDumpPolicy( ::fwMemory::policy::NeverDump::New() );
    // manager->setDumpPolicy( ::fwMemory::policy::ValveDump::New() );
    manager->setDumpPolicy( ::fwMemory::policy::AlwaysDump::New() );

    ::fwTools::IBufferManager::setCurrent( manager );
}

//------------------------------------------------------------------------------

void Plugin::stop() throw()
{
}

//------------------------------------------------------------------------------

} // namespace memory
