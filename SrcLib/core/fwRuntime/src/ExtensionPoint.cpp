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

#include "fwRuntime/ExtensionPoint.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/io/Validator.hpp"
#include "fwRuntime/RuntimeException.hpp"

namespace fwRuntime
{

//------------------------------------------------------------------------------

ExtensionPoint::ExtensionPoint( const std::shared_ptr< Bundle > bundle, const std::string& id,
                                const std::filesystem::path& schema ) :
    BundleElement( bundle ),
    m_id( id ),
    m_schema( schema )
{
}

//------------------------------------------------------------------------------

const std::string& ExtensionPoint::getIdentifier() const
{
    return m_id;
}

//------------------------------------------------------------------------------

std::shared_ptr< io::Validator > ExtensionPoint::getExtensionValidator() const
{
    if( !m_schema.empty() && !m_validator )
    {
        try
        {
            const std::filesystem::path schemaPath = getBundle()->getResourcesLocation() / m_schema;
            OSLM_DEBUG( "Use this schema : " << schemaPath << " for this id : " << m_id );
            m_validator = std::shared_ptr< io::Validator >( new io::Validator(schemaPath) );
        }
        catch( const std::exception& e )
        {
            throw RuntimeException( "Error while creating a validator. " + std::string(e.what()) );
        }
    }
    return m_validator;
}

//------------------------------------------------------------------------------

void ExtensionPoint::operator=( const ExtensionPoint& ) noexcept
{
}

//------------------------------------------------------------------------------

} // namespace fwRuntime
