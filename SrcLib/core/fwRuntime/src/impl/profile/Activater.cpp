/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwRuntime/impl/profile/Activater.hpp"

#include "fwRuntime/Extension.hpp"
#include "fwRuntime/impl/Bundle.hpp"
#include "fwRuntime/impl/ExtensionPoint.hpp"
#include "fwRuntime/Runtime.hpp"

namespace fwRuntime
{

namespace impl
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
    auto bundle = std::dynamic_pointer_cast< impl::Bundle >(Runtime::get().findBundle(m_identifier, m_version));

    SLM_FATAL_IF("Unable to activate Bundle " + m_identifier + "-" + m_version.string() + ". Not found.", bundle == 0);
    bundle->setEnable( true );

    // Managment of parameter configuration
    for( ParameterContainer::const_iterator i = m_parameters.begin();
         i != m_parameters.end();
         ++i )
    {
        bundle->addParameter( i->first, i->second );
    }

    // Disable extension point for this bundle
    for( DisableExtensionPointContainer::const_iterator id = m_disableExtensionPoints.begin();
         id != m_disableExtensionPoints.end();
         ++id )
    {
        if( bundle->hasExtensionPoint(*id) )
        {
            bundle->setEnableExtensionPoint( *id, false );
        }
        else
        {
            OSLM_ERROR(
                "Unable to disable Extension Point " << *id << " defined in the Bundle " << m_identifier <<
                    ". Not found.");
        }
    }

    // Disable extension for this bundle
    for( DisableExtensionContainer::const_iterator id = m_disableExtensions.begin();
         id != m_disableExtensions.end();
         ++id )
    {
        if( bundle->hasExtension(*id) )
        {
            bundle->setEnableExtension( *id, false );
        }
        else
        {
            OSLM_ERROR(
                "Unable to disable Extension " << *id << " defined in the Bundle " << m_identifier <<
                    ". Not found.");
        }
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace impl

} // namespace fwRuntime
