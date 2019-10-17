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

#include "fwRuntime/helper.hpp"

#include "fwRuntime/Convert.hpp"
#include <fwRuntime/impl/ExtensionPoint.hpp>
#include <fwRuntime/impl/Runtime.hpp>

#include <fwCore/base.hpp>

#include <iostream>

namespace fwRuntime
{

//------------------------------------------------------------------------------

void ConfigurationElement2XML(::fwRuntime::ConfigurationElement::sptr _cfgElement, xmlNodePtr pNode)
{
    //ATTRIBUTES + VALUES
    std::map<std::string, std::string> attr = _cfgElement->getAttributes();
    for ( std::map<std::string, std::string>::iterator iter_attr_cfe = attr.begin(); iter_attr_cfe != attr.end();
          ++iter_attr_cfe)
    {
        xmlSetProp(pNode, xmlCharStrdup((iter_attr_cfe->first).c_str()),
                   xmlCharStrdup((iter_attr_cfe->second).c_str()) );
    }
    //ELEMENTS
    for(::fwRuntime::ConfigurationElement::sptr elt :  _cfgElement->getElements())
    {
        xmlNodePtr child = xmlNewNode( NULL,  xmlCharStrdup( elt->getName().c_str() ) );

        xmlAddChild(pNode, child);
        // If configuration element is a XML_TEXT_NODE : WARNING : even whitespace (non XML_TEXT_NODE) have been
        // considered as valid XML_TEXT_NODE by ModuleDescriptorReader!!!!
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

ConfigurationElement::sptr getCfgAsAnExtension( ConfigurationElement::sptr config, std::string extension_pt )
{
    ConfigurationElement::sptr resultConfig;
    if( config->hasAttribute("config") )
    {
        std::string cfgContribution                    = config->getExistingAttributeValue("config");
        std::vector< ConfigurationElement::sptr > cfgs = ::fwRuntime::getAllCfgForPoint( extension_pt  );
        OSLM_FATAL_IF( "No configuration contribution found for extension point " <<  extension_pt, cfgs.empty());

        // Search for all matching contributions
        std::vector< ConfigurationElement::sptr > matchingCfg;
        for(ConfigurationElement::sptr elt :  cfgs)
        {
            if( cfgContribution == elt->getExistingAttributeValue("id") )
            {
                matchingCfg.push_back( elt );
            }
        }

        // If no contribution found
        OSLM_FATAL_IF( "No contribution matching the requested requirement (" << cfgContribution
                                                                              << " for extension point " << extension_pt << " )",
                       matchingCfg.empty());

        // Normal case : only one matching contribution has been found: matchingCfg.size == 1
        resultConfig = *matchingCfg.begin();

        // If several matching contributions
        OSLM_WARN_IF("Several contribution identified by " << cfgContribution << "( for cfg extension point " << extension_pt << " )"
                                                           << " has been found : the first one is returned",
                     (matchingCfg.size() > 1));
    }
    SLM_WARN_IF("Configuration has no config attribute",  !config->hasAttribute("config"));
    return resultConfig;
}

//------------------------------------------------------------------------------

std::vector< ConfigurationElement::sptr > getAllCfgForPoint( std::string _extension_pt )
{
    return ::fwRuntime::getAllConfigurationElementsForPoint(_extension_pt);
}

//------------------------------------------------------------------------------

std::vector< std::string > getAllIdsForPoint( std::string _extension_pt  )
{
    std::vector<std::string > ids;

    // Collects all contributed actions
    auto elements = ::fwRuntime::getAllConfigurationElementsForPoint( _extension_pt);

    // Creates all contributed action instances.
    for(const ::fwRuntime::ConfigurationElement::sptr& elt :  elements)
    {
        ids.push_back(elt->getAttributeValue("id"));
    }
    return ids;
}

//------------------------------------------------------------------------------

std::string getInfoForPoint( std::string _extension_pt  )
{
    std::string info = "";
    auto& rntm       = ::fwRuntime::impl::Runtime::get();
    if(rntm.findExtensionPoint( _extension_pt ) )
    {
        // Collects all contributed actions
        auto elements = ::fwRuntime::getAllConfigurationElementsForPoint( _extension_pt);

        // Creates all contributed action instances.
        for(const ::fwRuntime::ConfigurationElement::sptr& elt : elements)
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

std::map< std::string,
          ConfigurationElement::sptr > getAllIdAndConfigurationElementsForPoint( std::string _extension_pt )
{
    std::map<std::string, ConfigurationElement::sptr > cfgElementMap;

    // Collects all contributed actions
    auto elements = ::fwRuntime::getAllConfigurationElementsForPoint( _extension_pt);

    // Creates all contributed action instances.
    for(const ::fwRuntime::ConfigurationElement::sptr& elt : elements)
    {
        cfgElementMap[elt->getAttributeValue("id")] = elt;
    }
    return cfgElementMap;
}

//------------------------------------------------------------------------------

std::vector< std::shared_ptr< ::fwRuntime::Extension > > getAllExtensionsForPoint(std::string extension_pt)
{
    auto& rntm                                     = ::fwRuntime::impl::Runtime::get();
    std::shared_ptr< impl::ExtensionPoint >  point = rntm.findExtensionPoint(extension_pt);

    if( !point )
    {
        throw RuntimeException( extension_pt + ": invalid extension point identifier." );
    }

    return point->getAllExtensions();
}
}
