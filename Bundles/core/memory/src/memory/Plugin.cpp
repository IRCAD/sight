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

#include <fwMemory/Initializer.hpp>
#include <fwMemory/MemoryMonitor.hpp>
#include <fwMemory/IDumpPolicy.hpp>

#include "memory/Plugin.hpp"

namespace memory
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("memory::Plugin");

Plugin::~Plugin() throw()
{}

//------------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    ::fwTools::Factory::addInitializer( ::fwMemory::Initializer::New() ) ;

    if( this->getBundle()->hasParameter("Policy") )
    {
        SLM_INFO("Memory policy specified by monitor component") ;
        std::string policyName = this->getBundle()->getParameterValue("Policy") ;
        typedef std::map< std::string , ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > > MapType ;
        MapType ids = ::fwRuntime::getAllIdAndConfigurationElementsForPoint( "::memory::DumpPolicy" ) ;
        if( ids.find(policyName) != ids.end() )
        {
            ::boost::shared_ptr< ::fwMemory::IDumpPolicy > memPolicy = ::fwTools::ClassFactoryRegistry::create< ::fwMemory::IDumpPolicy , std::string >( policyName ) ;
            SLM_INFO("Affecting Memory policy to the memory monitor") ;
            ::fwMemory::MemoryMonitor::setPolicy( memPolicy ) ;
            memPolicy->configure( ids[policyName] );
        }
        else
        {
            SLM_WARN("Bad memory policy identifier") ;
        }
    }
}

//------------------------------------------------------------------------------

void Plugin::stop() throw()
{
    ::fwTools::System::getDefault()->eraseDumpFolderOfZombies();
}

//------------------------------------------------------------------------------

} // namespace memory
