/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/Executable.hpp"
#include "fwRuntime/ConfigurationElement.hpp"


namespace fwRuntime
{

//------------------------------------------------------------------------------

std::shared_ptr<Bundle> Executable::getBundle() const noexcept
{
    return m_bundle;
}

//------------------------------------------------------------------------------

const ConfigurationElement::sptr Executable::getInitializationData() const noexcept
{
    return m_initializationData;
}

//------------------------------------------------------------------------------

void Executable::setBundle( std::shared_ptr< Bundle > bundle )
{
    m_bundle = bundle;
}

//------------------------------------------------------------------------------

void Executable::setInitializationData( const ConfigurationElement::sptr element ) noexcept
{
    m_initializationData = element;
}

//------------------------------------------------------------------------------

} // namespace fwRuntime
