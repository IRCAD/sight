/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif
#include <iostream>
#include <sstream>
#include <libxml/xmlstring.h>
#if defined(__GNUC__)
#pragma GCC visibility pop
#endif

#include <fwCore/base.hpp>

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/ExtensionPoint.hpp"
#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/io/Validator.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/helper.hpp"

namespace fwRuntime
{

//------------------------------------------------------------------------------

Extension::Extension( ::boost::shared_ptr< Bundle > bundle, const std::string & id, const std::string & point, xmlNodePtr xmlNode )
: BundleElement ( bundle                    ),
  m_id          ( id                        ),
  m_point       ( point                     ),
  m_xmlDoc      ( xmlNewDoc(BAD_CAST "1.0") ),
  m_xmlNode     ( xmlCopyNode(xmlNode, 1)   ),
  m_validity    ( UnknownValidity           )
{
    xmlDocSetRootElement(m_xmlDoc, m_xmlNode);
}

//------------------------------------------------------------------------------

Extension::~Extension()
{
    xmlFreeDoc( m_xmlDoc );
}

//------------------------------------------------------------------------------

const std::string& Extension::getIdentifier() const
{
    return m_id;
}

//------------------------------------------------------------------------------

const std::string& Extension::getPoint() const
{
    return m_point;
}

//------------------------------------------------------------------------------

xmlNodePtr Extension::getXmlNode() const
{
    return m_xmlNode;
}

//------------------------------------------------------------------------------

const Extension::Validity Extension::getValidity() const
{
    return m_validity;
}

//------------------------------------------------------------------------------

const Extension::Validity Extension::validate()
{
    // Skips the validation if already done.
    if( m_validity != UnknownValidity )
    {
        return m_validity;
    }

    // Retrieves the extension point.
    Runtime * rntm( Runtime::getDefault() );
     ::boost::shared_ptr< ExtensionPoint >  point( rntm->findExtensionPoint(m_point) );

    // Checks that the point exists.
    if( !point )
    {
        throw RuntimeException(m_point + " : invalid point reference.");
    }

    // Validates the extension.
    ::boost::shared_ptr< io::Validator >   validator( point->getExtensionValidator() );
    OSLM_ASSERT("Sorry, validator creation failed for point "<<point->getIdentifier(), validator );

    // Check extension XML Node <extension id="xxx" implements="yyy" >...</extension>
    validator->clearErrorLog();
    if( validator->validate( m_xmlNode ) == true )
    {
        m_validity = Valid;
    }
    else
    {
        m_validity = Invalid;
        const std::string   identifier = m_id.empty() ? "anonymous" : m_id;
        OSLM_ERROR("In bundle " << getBundle()->getIdentifier() << ". " << identifier
                << ": invalid extension XML element node does not respect schema. Verification error log is : "
                << std::endl << validator->getErrorLog() );
    }

    return m_validity;
}

//------------------------------------------------------------------------------

void Extension::operator=(const Extension&) throw()
{}

//------------------------------------------------------------------------------

} // namespace fwRuntime
