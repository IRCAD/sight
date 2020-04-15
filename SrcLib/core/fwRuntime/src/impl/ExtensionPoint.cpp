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

#include "fwRuntime/impl/ExtensionPoint.hpp"

#include "fwRuntime/impl/io/Validator.hpp"
#include "fwRuntime/Module.hpp"
#include "fwRuntime/operations.hpp"
#include "fwRuntime/RuntimeException.hpp"

namespace fwRuntime
{

namespace impl
{

//------------------------------------------------------------------------------

ExtensionPoint::ExtensionPoint( const std::shared_ptr< Module > bundle, const std::string& id,
                                const std::filesystem::path& schema ) :
    ModuleElement( bundle ),
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
            std::filesystem::path schemaPath = getModule()->getResourcesLocation() / m_schema;
            OSLM_DEBUG( "Use this schema : " << schemaPath << " for this id : " << m_id );
            if(!std::filesystem::exists(schemaPath))
            {
                // Allow to specify a schema defined elsewhere than this module
                schemaPath = ::fwRuntime::getResourceFilePath(m_schema);
            }
            m_validator = std::make_shared< io::Validator >(schemaPath);
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

} // namespace impl

} // namespace fwRuntime
