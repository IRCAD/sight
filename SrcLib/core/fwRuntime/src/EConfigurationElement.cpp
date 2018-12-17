/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwRuntime/EConfigurationElement.hpp"

#include "fwRuntime/Bundle.hpp"

namespace fwRuntime
{

EConfigurationElement::EConfigurationElement(const std::string& name) :
    ::fwRuntime::ConfigurationElement( std::shared_ptr< ::fwRuntime::Bundle >(), name )
{
}

EConfigurationElement::~EConfigurationElement()
{
}

//------------------------------------------------------------------------------

EConfigurationElement::sptr EConfigurationElement::addConfigurationElement( const std::string& name )
{
    EConfigurationElement::sptr cfg = EConfigurationElement::New(name);
    this->::fwRuntime::ConfigurationElement::addConfigurationElement( cfg );
    return cfg;
}

//------------------------------------------------------------------------------

void EConfigurationElement::addConfigurationElement( EConfigurationElement::sptr element )
{
    this->::fwRuntime::ConfigurationElement::addConfigurationElement( element );
}

//------------------------------------------------------------------------------

void EConfigurationElement::setAttributeValue(const std::string& name, const std::string& value) noexcept
{
    this->::fwRuntime::ConfigurationElement::setAttributeValue( name, value );
}

//------------------------------------------------------------------------------

void EConfigurationElement::setValue(const std::string& value) noexcept
{
    this->::fwRuntime::ConfigurationElement::setValue(value);
}

}
