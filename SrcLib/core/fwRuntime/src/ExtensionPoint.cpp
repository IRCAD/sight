/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/ExtensionPoint.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/io/Validator.hpp"
#include "fwRuntime/RuntimeException.hpp"

namespace fwRuntime
{

//------------------------------------------------------------------------------

ExtensionPoint::ExtensionPoint( const std::shared_ptr< Bundle > bundle, const std::string& id,
                                const ::boost::filesystem::path& schema ) :
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
            const ::boost::filesystem::path schemaPath = getBundle()->getResourcesLocation() / m_schema;
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
