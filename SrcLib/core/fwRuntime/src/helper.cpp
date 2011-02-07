/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/helper.hpp"
#include "fwRuntime/Convert.hpp"
#include <iostream>
#include <fwCore/base.hpp>


namespace fwRuntime
{

//------------------------------------------------------------------------------

std::pair< bool , std::string > validateConfigurationElement( ::boost::shared_ptr< ::fwRuntime::io::Validator > _validator , ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _element )
{
    assert( _validator ) ;
    assert( _element ) ;

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

void ConfigurationElement2XML(::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement, xmlNodePtr pNode)
{
    //ATTRIBUTES + VALUES
    std::map<std::string, std::string> attr = _cfgElement->getAttributes() ;
    for ( std::map<std::string, std::string>::iterator iter_attr_cfe = attr.begin() ; iter_attr_cfe!= attr.end(); ++iter_attr_cfe)
    {
        xmlSetProp(pNode , xmlCharStrdup((iter_attr_cfe->first).c_str()), xmlCharStrdup((iter_attr_cfe->second).c_str()) ) ;
    }
    //ELEMENTS
    for ( std::vector< ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > >::iterator iter = _cfgElement->begin() ; iter != _cfgElement->end() ; ++iter )
    {
        xmlNodePtr child = xmlNewNode( NULL,  xmlCharStrdup( (*iter)->getName().c_str() ) );

        xmlAddChild(pNode, child ) ;
        // If configuration element is a XML_TEXT_NODE : WARNING : even whitespace (non XML_TEXT_NODE) have been considered as valid XML_TEXT_NODE by BundleDescriptorReader!!!!
        if( !(*iter)->getValue().empty() )
        {
            xmlNodeSetContent( child , xmlCharStrdup( (*iter)->getValue().c_str() ) ) ;
        }
        // If configuration element is a XML_ELEMENT_NODE
        else
        {
            ConfigurationElement2XML( (*iter), child) ;
        }
    }

}

//------------------------------------------------------------------------------

::boost::shared_ptr< ConfigurationElement > getCfgAsAnExtension( ::boost::shared_ptr< ConfigurationElement > _config , std::string _extension_pt )
{
    if( _config->hasAttribute("config") )
    {
        std::string cfgContribution = _config->getExistingAttributeValue("config") ;
        std::vector< ::boost::shared_ptr< ConfigurationElement > > cfgs = ::fwRuntime::getAllCfgForPoint( _extension_pt  ) ;
        if( cfgs.empty() )
        {
            OSLM_FATAL( "No configuration contribution found for extension point " <<  _extension_pt);
            return ::boost::shared_ptr< ConfigurationElement >() ;
        }

        assert( !cfgs.empty() ) ; // the proposed configuration has not been found
        std::vector< ::boost::shared_ptr< ConfigurationElement > >::iterator iter ;
        // Search for all matching contributions
        std::vector< ::boost::shared_ptr< ConfigurationElement > > matchingCfg ;
        for( iter = cfgs.begin() ; iter != cfgs.end() ; ++iter )
        {
            if( cfgContribution == (*iter)->getExistingAttributeValue("id") )
            {
                matchingCfg.push_back( *iter ) ;
            }
        }
        // Normal case : only one matching contribution has been found
        if( matchingCfg.size() == 1 )
        {
            return  *matchingCfg.begin() ;
        }
        // If several matching contributions
        if( matchingCfg.size() > 1 )
        {
            std::stringstream msg;
            msg << "Several contribution identified by " << cfgContribution << "( for cfg extension point " << _extension_pt << " )" << " has been found : the first one is returned";
            OSLM_WARN( msg.str() );

            return  *matchingCfg.begin() ;
        }
        // If no contribution found
        OSLM_FATAL( "No contribution matching the requested requirement (" << cfgContribution << " for extension point " << _extension_pt << " )" );
        return ::boost::shared_ptr< ConfigurationElement >() ;
    }
    else
    {
        OSLM_WARN("Configuration has no config attribute" );
        return ::boost::shared_ptr< ConfigurationElement >() ;
    }
    assert(false) ;
    return ::boost::shared_ptr< ConfigurationElement >() ;
}

//------------------------------------------------------------------------------

std::vector< ::boost::shared_ptr< ConfigurationElement > > getAllCfgForPoint( std::string _extension_pt )
{
    std::vector< ::boost::shared_ptr< ConfigurationElement > > cfgElements ;

    using ::fwRuntime::ConfigurationElement;

    typedef std::vector< ::boost::shared_ptr< ConfigurationElement > > ElementContainer;
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
    typedef std::vector< ::boost::shared_ptr< ConfigurationElement > > ElementContainer;
    typedef std::back_insert_iterator< ElementContainer > Inserter;

    // Collects all contribued actions
    ElementContainer    elements;
    Inserter            inserter(elements);
    ::fwRuntime::getAllConfigurationElementsForPoint( _extension_pt , inserter);

    // Creates all contributed action instances.
    ElementContainer::const_iterator    iter;
    for(iter = elements.begin(); iter != elements.end(); ++iter)
    {
        std::string identifier = (*iter)->getAttributeValue("id");
        ids.push_back(identifier) ;
//      cfgElementMap[identifier]=*iter;
    }
    return ids ;
}

//------------------------------------------------------------------------------

std::string getInfoForPoint( std::string _extension_pt  )
{
    if( ! ::fwRuntime::findExtensionPoint( _extension_pt ) )
    {
        return "";
    }

    std::vector<std::string > ids ;

    using ::fwRuntime::ConfigurationElement;
    typedef std::vector< ::boost::shared_ptr< ConfigurationElement > > ElementContainer;
    typedef std::back_insert_iterator< ElementContainer > Inserter;

    // Collects all contribued actions
    ElementContainer    elements;
    Inserter            inserter(elements);
    ::fwRuntime::getAllConfigurationElementsForPoint( _extension_pt , inserter);

    // Creates all contributed action instances.
    ElementContainer::const_iterator    iter;
    for(iter = elements.begin(); iter != elements.end(); ++iter)
    {
        if( (*iter)->getName() == "info" && (*iter)->hasAttribute("text") )
        {
            return (*iter)->getAttributeValue("text");
        }
    }
    return "" ;
}

//------------------------------------------------------------------------------

std::map< std::string , ::boost::shared_ptr< ConfigurationElement > > getAllIdAndConfigurationElementsForPoint( std::string _extension_pt )
{
    std::map<std::string, ::boost::shared_ptr< ConfigurationElement > > cfgElementMap ;

    using ::fwRuntime::ConfigurationElement;
    typedef std::vector< ::boost::shared_ptr< ConfigurationElement > > ElementContainer;
    typedef std::back_insert_iterator< ElementContainer > Inserter;

    // Collects all contribued actions
    ElementContainer    elements;
    Inserter            inserter(elements);
    ::fwRuntime::getAllConfigurationElementsForPoint( _extension_pt , inserter);

    // Creates all contributed action instances.
    ElementContainer::const_iterator    iter;
    for(iter = elements.begin(); iter != elements.end(); ++iter)
    {
        std::string identifier = (*iter)->getAttributeValue("id");
        cfgElementMap[identifier]=*iter;
    }
    return cfgElementMap ;
}

//------------------------------------------------------------------------------

}
