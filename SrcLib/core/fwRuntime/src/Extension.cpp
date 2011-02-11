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



Extension::~Extension()
{
    xmlFreeDoc( m_xmlDoc );
}



const std::string& Extension::getIdentifier() const
{
    return m_id;
}



const std::string& Extension::getPoint() const
{
    return m_point;
}



xmlNodePtr Extension::getXmlNode() const
{
    return m_xmlNode;
}



const Extension::Validity Extension::getValidity() const
{
    return m_validity;
}



const Extension::Validity Extension::validate()
{
    // Skips the validation if already done.
    if( m_validity != UnknownValidity )
    {
        return m_validity;
    }


    // Retrieves the extension point.
    Runtime                     * rntm( Runtime::getDefault() );
     ::boost::shared_ptr< ExtensionPoint >  point( rntm->findExtensionPoint(m_point) );

    // Checks that the point exists.
    if( !point )
    {
        throw RuntimeException(m_point + " : invalid point reference.");
    }


    // Validates the extension.
    ::boost::shared_ptr< io::Validator >   validator( point->getExtensionValidator() );
    SLM_ASSERT("Sorry, validator creation failed", validator );

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
                << ": invalid extension because both root and children XML element node does not respect schema. Verification error log is : "
                << std::endl << validator->getErrorLog() );
    }

    return m_validity;




     /*


    // The validator is defined by a xsd schema specified by the extension point to which this extension contributes
    // The xsd schema can have been build to check the whole extension or each of its children xml element nodes
    // Validation is true if global validation is true or if each children is true
    // Validation is only performed if there is a validator and if there are children
    bool isThereChildren = ( this->begin() != this->end() ) ;
    if( validator && isThereChildren )
    {
        // Check extension XML Node <extension id="xxx" implements="yyy" >...</extension>
        std::pair< bool , std::string > extensionRootValidationResult;
        validator->clearErrorLog();
        if( validator->validate( m_xmlNode ) == true )
        {
            extensionRootValidationResult.first = true ;
        }
        else
        {
            extensionRootValidationResult.first = false ;
            extensionRootValidationResult.second = validator->getErrorLog() ;
        }
        // Check each extension XML children element node
        // Additional validation procedure : check all XML child nodes of <extension id="xxx" implements="yyy" >...</extension>
        std::vector< std::pair< bool , std::string > > invalidExtensionChildren;
        for(ConfigurationElementContainer::Iterator iter = this->begin() ; iter != this->end() ; ++iter )
        {
            const std::string   identifier = m_id.empty() ? "anonymous" : m_id;
            std::pair< bool , std::string > extensionChildValidity = ::fwRuntime::validateConfigurationElement( validator , *iter ) ;
            if( extensionChildValidity.first == false )
            {
                invalidExtensionChildren.push_back( extensionChildValidity ) ;
//              OSLM_TRACE("In bundle " << getBundle()->getIdentifier() << ". " << identifier << ": checking configuration element " << (*iter)->getName() << " : NOK");
            }
            else
            {
//              OSLM_TRACE("In bundle " << getBundle()->getIdentifier() << ". " << identifier << ": checking configuration element " << (*iter)->getName() << " : OK");
            }
        }

//      xmlNodePtr curChild;
//      for(curChild = m_xmlNode->children; curChild != 0; curChild = curChild->next)
//      {
//          if(curChild->type == XML_ELEMENT_NODE)
//          {
//              xmlDocPtr xmlDoc    = xmlNewDoc(BAD_CAST "1.0") ;
//              xmlNodePtr xmlNode  = xmlCopyNode(curChild, 1) ;
//              xmlDocSetRootElement(xmlDoc, xmlNode);
//
//              const std::string   identifier = m_id.empty() ? "anonymous" : m_id;
//              std::string name((const char*) curChild->name);
//              OSLM_INFO("In bundle " << getBundle()->getIdentifier() << ". " << identifier << ": checking configuration element. " << name );
//              validator->clearErrorLog();
//              if( !( validator->validate( xmlNode ) == true ) )
//              {
//                  std::pair< bool , std::string > invalidExtensionChild;
//                  invalidExtensionChild.first     = false ;
//                  invalidExtensionChild.second    = validator->getErrorLog() ;
//                  invalidExtensionChildren.push_back( invalidExtensionChild ) ;
//              }
//              xmlFreeDoc( xmlDoc );
//          }
//      }

        // Analyze the whole verification procedure results
        // If there are several children and if there are all valid vs schema:
        // This means that the schema was dedicated to them: one consider that extension is valid
        if( invalidExtensionChildren.empty() )
        {
            m_validity = Valid;
        }
        // If there are several children and if some of them are invalid vs schema:
        else
        {
            // First case : root is valid. This means that schema was dedicated to rootExtension
            if( extensionRootValidationResult.first )
            {
                m_validity = Valid;
            }
            // Else : both root and children are invalid.
            else
            {
                std::stringstream errors ;
                errors << extensionRootValidationResult.second << std::endl;
                for( std::vector< std::pair< bool , std::string > >::iterator iter = invalidExtensionChildren.begin() ; iter != invalidExtensionChildren.end() ; ++iter )
                {
                    errors << iter->second << std::endl;
                }
                const std::string   identifier = m_id.empty() ? "anonymous" : m_id;
                OSLM_ERROR("In bundle " << getBundle()->getIdentifier() << ". " << identifier << ": invalid extension because both root and children XML element node does not respect schema. Verification error log is : " << std::endl << errors.str() );
                m_validity = Invalid;
            }
        }
        // If there are several children and if there are all valid vs schema:
        // This means that the schema was dedicated to them: one consider that extension is valid
    }
    else
    {
        m_validity = Valid;
    }

    return m_validity ;
    */
}

void Extension::operator=(const Extension&) throw()
{}



} // namespace fwRuntime
