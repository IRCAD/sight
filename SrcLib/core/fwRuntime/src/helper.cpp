/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include "fwRuntime/helper.hpp"
#include "fwRuntime/Convert.hpp"

namespace fwRuntime
{

//------------------------------------------------------------------------------

std::pair< bool , std::string > validateConfigurationElement( ::boost::shared_ptr< ::fwRuntime::io::Validator > _validator , ::fwRuntime::ConfigurationElement::sptr _element )
{
    SLM_ASSERT("_validator not instanced", _validator);
    SLM_ASSERT("_element not instanced", _element);

    xmlNodePtr _elementNodePtr = xmlNewNode( NULL,  xmlCharStrdup( _element->getName().c_str() ) );
    ::fwRuntime::ConfigurationElement2XML( _element , _elementNodePtr ) ;
    xmlDocPtr xmlDoc    = xmlNewDoc(BAD_CAST "1.0") ;
    xmlNodePtr xmlNode  = xmlCopyNode(_elementNodePtr, 1) ;
    xmlDocSetRootElement(xmlDoc, xmlNode);

    _validator->clearErrorLog();

    std::pair< bool , std::string > validationResult;
    if( !(_validator->validate( xmlNode ) == true) )
    {
        validationResult.first  = false ;
        validationResult.second = _validator->getErrorLog() ;
    }
    else
    {
        validationResult.first  = true ;
    }

    xmlFreeDoc( xmlDoc );
    return validationResult ;
}

//------------------------------------------------------------------------------

void ConfigurationElement2XML(::fwRuntime::ConfigurationElement::sptr _cfgElement, xmlNodePtr pNode)
{
    //ATTRIBUTES + VALUES
    std::map<std::string, std::string> attr = _cfgElement->getAttributes() ;
    for ( std::map<std::string, std::string>::iterator iter_attr_cfe = attr.begin() ; iter_attr_cfe!= attr.end(); ++iter_attr_cfe)
    {
        xmlSetProp(pNode , xmlCharStrdup((iter_attr_cfe->first).c_str()), xmlCharStrdup((iter_attr_cfe->second).c_str()) ) ;
    }
    //ELEMENTS
    BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr elt, _cfgElement->getElements())
    {
        xmlNodePtr child = xmlNewNode( NULL,  xmlCharStrdup( elt->getName().c_str() ) );

        xmlAddChild(pNode, child) ;
        // If configuration element is a XML_TEXT_NODE : WARNING : even whitespace (non XML_TEXT_NODE) have been considered as valid XML_TEXT_NODE by BundleDescriptorReader!!!!
        if( !elt->getValue().empty() )
        {
            xmlNodeSetContent(child, xmlCharStrdup( elt->getValue().c_str() ));
        }
        // If configuration element is a XML_ELEMENT_NODE
        else
        {
            ConfigurationElement2XML(elt, child);
        }
    }
}

//------------------------------------------------------------------------------

ConfigurationElement::sptr getCfgAsAnExtension( ConfigurationElement::sptr config , std::string extension_pt )
{
    ConfigurationElement::sptr resultConfig;
    if( config->hasAttribute("config") )
    {
        std::string cfgContribution = config->getExistingAttributeValue("config") ;
        std::vector< ConfigurationElement::sptr > cfgs = ::fwRuntime::getAllCfgForPoint( extension_pt  ) ;
        OSLM_FATAL_IF( "No configuration contribution found for extension point " <<  extension_pt, cfgs.empty());

        // Search for all matching contributions
        std::vector< ConfigurationElement::sptr > matchingCfg ;
        BOOST_FOREACH(ConfigurationElement::sptr elt, cfgs)
        {
            if( cfgContribution == elt->getExistingAttributeValue("id") )
            {
                matchingCfg.push_back( elt ) ;
            }
        }

        // If no contribution found
        OSLM_FATAL_IF( "No contribution matching the requested requirement (" << cfgContribution
                << " for extension point " << extension_pt << " )", matchingCfg.empty());

        // Normal case : only one matching contribution has been found: matchingCfg.size == 1
        resultConfig =  *matchingCfg.begin();

        // If several matching contributions
        OSLM_WARN_IF("Several contribution identified by " << cfgContribution << "( for cfg extension point " << extension_pt << " )"
                << " has been found : the first one is returned", (matchingCfg.size() > 1));
    }
    SLM_WARN_IF("Configuration has no config attribute",  !config->hasAttribute("config"));
    return resultConfig ;
}

//------------------------------------------------------------------------------

std::vector< ConfigurationElement::sptr > getAllCfgForPoint( std::string _extension_pt )
{
    using ::fwRuntime::ConfigurationElement;

    typedef std::vector< ConfigurationElement::sptr > ElementContainer;
    typedef std::back_insert_iterator< ElementContainer > Inserter;

    ElementContainer    renderElements;
    Inserter            renderInserter(renderElements);

    ::fwRuntime::getAllConfigurationElementsForPoint(_extension_pt, renderInserter);
    return renderElements ;
}

//------------------------------------------------------------------------------

std::vector< std::string > getAllIdsForPoint( std::string _extension_pt  )
{
    std::vector<std::string > ids ;

    using ::fwRuntime::ConfigurationElement;
    typedef std::vector< ConfigurationElement::sptr > ElementContainer;
    typedef std::back_insert_iterator< ElementContainer > Inserter;

    // Collects all contributed actions
    ElementContainer    elements;
    Inserter            inserter(elements);
    ::fwRuntime::getAllConfigurationElementsForPoint( _extension_pt , inserter);

    // Creates all contributed action instances.
    BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr elt, elements)
    {
        ids.push_back(elt->getAttributeValue("id")) ;
    }
    return ids ;
}

//------------------------------------------------------------------------------

std::string getInfoForPoint( std::string _extension_pt  )
{
    std::string info = "";
    if(::fwRuntime::findExtensionPoint( _extension_pt ) )
    {
        using ::fwRuntime::ConfigurationElement;
        typedef std::vector< ConfigurationElement::sptr > ElementContainer;
        typedef std::back_insert_iterator< ElementContainer > Inserter;

        // Collects all contributed actions
        ElementContainer    elements;
        Inserter            inserter(elements);
        ::fwRuntime::getAllConfigurationElementsForPoint( _extension_pt, inserter);

        // Creates all contributed action instances.
        BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr elt, elements)
        {
            if( elt->getName() == "info" && elt->hasAttribute("text") )
            {
                info = elt->getAttributeValue("text");
                break;
            }
        }
    }
    return info;
}

//------------------------------------------------------------------------------

std::map< std::string , ConfigurationElement::sptr > getAllIdAndConfigurationElementsForPoint( std::string _extension_pt )
{
    std::map<std::string, ConfigurationElement::sptr > cfgElementMap ;

    using ::fwRuntime::ConfigurationElement;
    typedef std::vector< ConfigurationElement::sptr > ElementContainer;
    typedef std::back_insert_iterator< ElementContainer > Inserter;

    // Collects all contributed actions
    ElementContainer    elements;
    Inserter            inserter(elements);
    ::fwRuntime::getAllConfigurationElementsForPoint( _extension_pt, inserter);

    // Creates all contributed action instances.
    BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr elt, elements)
    {
        cfgElementMap[elt->getAttributeValue("id")]= elt;
    }
    return cfgElementMap ;
}

//------------------------------------------------------------------------------

std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > getAllExtensionsForPoint(std::string extension_pt)
{
    typedef std::vector< ::boost::shared_ptr< Extension > > ExtensionContainer;
    typedef std::back_insert_iterator< ExtensionContainer > Inserter;

    ExtensionContainer  extElements;
    Inserter            extInserter(extElements);

    ::fwRuntime::getAllExtensionsForPoint ( extension_pt , extInserter );
    return extElements;
}
}
