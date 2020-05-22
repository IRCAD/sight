/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwRuntime/detail/profile/Activater.hpp"

#include "fwRuntime/detail/ExtensionPoint.hpp"
#include "fwRuntime/detail/Module.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/Runtime.hpp"

namespace fwRuntime
{

namespace detail
{

namespace profile
{

//------------------------------------------------------------------------------

Activater::Activater( const std::string& identifier, const std::string& version ) :
    m_identifier( identifier ),
    m_version( version )
{
}

//------------------------------------------------------------------------------

void Activater::addParameter( const std::string& identifier, const std::string& value )
{
    m_parameters[identifier] = value;
}

//------------------------------------------------------------------------------

void Activater::addDisableExtensionPoint( const std::string& identifier )
{
    m_disableExtensionPoints.push_back(identifier);
}

//------------------------------------------------------------------------------

void Activater::addDisableExtension( const std::string& identifier )
{
    m_disableExtensions.push_back(identifier);
}

//------------------------------------------------------------------------------

void Activater::apply()
{
    auto module = std::dynamic_pointer_cast< detail::Module >(Runtime::get().findModule(m_identifier, m_version));

    SLM_FATAL_IF("Unable to activate Module " + m_identifier + "-" + m_version.string() + ". Not found.", module == 0);
    module->setEnable( true );

    // Managment of parameter configuration
    for( ParameterContainer::const_iterator i = m_parameters.begin();
         i != m_parameters.end();
         ++i )
    {
        module->addParameter( i->first, i->second );
    }

    // Disable extension point for this module
    for( DisableExtensionPointContainer::const_iterator id = m_disableExtensionPoints.begin();
         id != m_disableExtensionPoints.end();
         ++id )
    {
        if( module->hasExtensionPoint(*id) )
        {
            module->setEnableExtensionPoint( *id, false );
        }
        else
        {
            OSLM_ERROR(
                "Unable to disable Extension Point " << *id << " defined in the Module " << m_identifier <<
                    ". Not found.");
        }
    }

    // Disable extension for this module
    for( DisableExtensionContainer::const_iterator id = m_disableExtensions.begin();
         id != m_disableExtensions.end();
         ++id )
    {
        if( module->hasExtension(*id) )
        {
            module->setEnableExtension( *id, false );
        }
        else
        {
            OSLM_ERROR(
                "Unable to disable Extension " << *id << " defined in the Module " << m_identifier <<
                    ". Not found.");
        }
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace detail

} // namespace fwRuntime
