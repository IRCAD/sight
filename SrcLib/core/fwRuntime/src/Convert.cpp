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

#include "fwRuntime/Convert.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/ExtensionPoint.hpp"
#include "fwRuntime/io/BundleDescriptorReader.hpp"
#include "fwRuntime/Runtime.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <libxml/parser.h>

#include <set>

namespace fwRuntime
{

//------------------------------------------------------------------------------

Convert::Convert()
{
}

//------------------------------------------------------------------------------

Convert::~Convert()
{
}

//------------------------------------------------------------------------------

void Convert::fromConfigurationElementToXml( std::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement,
                                             xmlNodePtr _node)
{
    //NAME
    xmlNodePtr pNode = xmlNewNode( NULL,  xmlCharStrdup( _cfgElement->getName().c_str() ) );
    xmlAddChild(_node, pNode );

    std::string nodeValue = _cfgElement->getValue();
    if(!nodeValue.empty())
    {
        xmlNodeSetContent(pNode, reinterpret_cast<const xmlChar*>(nodeValue.c_str()));
    }

    std::map<std::string, std::string> attr_cfe = _cfgElement->getAttributes();

    for ( std::map<std::string, std::string>::iterator iter_attr_cfe = attr_cfe.begin();
          iter_attr_cfe != attr_cfe.end();
          ++iter_attr_cfe)
    {
        //ATTRIBUTES + VALUES
        xmlSetProp(pNode, xmlCharStrdup((iter_attr_cfe->first).c_str()),
                   xmlCharStrdup((iter_attr_cfe->second).c_str()) );
        if ((iter_attr_cfe->first) == std::string("class"))
        {
            xmlSetProp(pNode, xmlCharStrdup((iter_attr_cfe->first).c_str()),
                       xmlCharStrdup((iter_attr_cfe->second).c_str()) );
        }
    }

    SLM_ASSERT( "ConfigurationElement should not have children("
                << _cfgElement->size() <<") and a value ("
                << nodeValue <<") at the same time.",
                !(!nodeValue.empty() && _cfgElement->size())  );

    for ( std::vector< ::fwRuntime::ConfigurationElement::sptr >::iterator iter_cfeC = _cfgElement->begin();
          iter_cfeC != _cfgElement->end();
          ++iter_cfeC )
    {
        fromConfigurationElementToXml( (*iter_cfeC), pNode);
    }

}

//------------------------------------------------------------------------------

xmlNodePtr Convert::runningBundlesToXml( )
{
    xmlNodePtr node_root = xmlNewNode( NULL,  xmlCharStrdup( BUNDLE_RC_PREFIX ) );
    std::set< std::shared_ptr< ::fwRuntime::Bundle > > ::iterator iter_bundles;
    ::fwRuntime::Runtime* tmp_runtime = ::fwRuntime::Runtime::getDefault();

    xmlNodePtr activated_Node = xmlNewNode( NULL,  xmlCharStrdup( "Activated" ) );
    xmlAddChild(node_root, activated_Node );

    xmlNodePtr inactivated_Node = xmlNewNode( NULL,  xmlCharStrdup( "Inactivated" ) );
    xmlAddChild(node_root, inactivated_Node );

    bool enable_Value = false;  // the 'do while' loop stop if enable_Value==false.
    do
    {
        enable_Value = !enable_Value;
        for (iter_bundles = tmp_runtime->bundlesBegin();
             iter_bundles != tmp_runtime->bundlesEnd();
             ++iter_bundles)
        {
            //BUNDLE

            xmlNodePtr bundleNode = xmlNewNode( NULL, xmlCharStrdup( (*iter_bundles)->getIdentifier().c_str() ) );
            if (enable_Value)
            {
                xmlAddChild(activated_Node, bundleNode );
            }
            else
            {
                xmlAddChild(inactivated_Node, bundleNode );
            }

            //EXTENSIONS POINTS
            xmlNodePtr extensionPoint_activated_list_Node = xmlNewNode( NULL,  xmlCharStrdup( "Extensions_Points" ) );
            xmlAddChild(bundleNode, extensionPoint_activated_list_Node );

            for ( std::set< std::shared_ptr< ::fwRuntime::ExtensionPoint > >::const_iterator iter_extensionPoints =
                      (*iter_bundles)->extensionPointsBegin();
                  iter_extensionPoints != (*iter_bundles)->extensionPointsEnd();
                  ++iter_extensionPoints)
            {
                //EXTENSIONS POINTS
                std::string str = "Identifier : "+(*iter_extensionPoints)->getIdentifier();
                //-----DEBUG------
                xmlNodePtr extensionPointsNode = xmlNewNode( NULL,  xmlCharStrdup( str.c_str() ) );
                //xmlNodePtr extensionPointsNode = xmlNewNode( NULL,  xmlCharStrdup(
                // (str.substr(str.find_last_of("::")+1)).c_str() ) ) ;
                //-----DEBUG------
                if (((*iter_extensionPoints)->isEnable()) == enable_Value)
                {
                    xmlAddChild(extensionPoint_activated_list_Node, extensionPointsNode );
                }

                std::vector< ::fwRuntime::ConfigurationElement::sptr > AllConfigElement =
                    ((*iter_extensionPoints)->getAllConfigurationElements());
                for (std::vector< ::fwRuntime::ConfigurationElement::sptr >::const_iterator iter_config_elem =
                         AllConfigElement.begin();
                     iter_config_elem != AllConfigElement.end();
                     ++iter_config_elem)
                {
                    //CONFIGURATIONS ELEMENTS
                    Convert::fromConfigurationElementToXml( (*iter_config_elem), extensionPointsNode);
                }
            }//end extensionsPoints parsing

            //Extensions
            xmlNodePtr extension_activated_list_Node = xmlNewNode( NULL,  xmlCharStrdup( "Extensions" ) );
            xmlAddChild(bundleNode, extension_activated_list_Node );

            for ( std::set< std::shared_ptr< ::fwRuntime::Extension > >::const_iterator iter_extension =
                      (*iter_bundles)->extensionsBegin();
                  iter_extension != (*iter_bundles)->extensionsEnd();
                  ++iter_extension)
            {
                if (((*iter_extension)->isEnable()) == enable_Value)
                {
                    std::string str          = (*iter_extension)->getPoint();
                    xmlNodePtr extensionNode = xmlNewNode( NULL,  xmlCharStrdup( (str.c_str() ) ) );

                    // Adds node if not exist
                    xmlNodePtr node;
                    bool found_node = false;
                    for ( node = extension_activated_list_Node->children;
                          node;
                          node = node->next)
                    {
                        if ( (node->type == XML_ELEMENT_NODE) &&
                             !(xmlStrcmp(node->name, xmlCharStrdup( (str.c_str() ) ) ) ) )
                        {
                            extensionNode = node;
                            found_node    = true;
                            break;
                        }
                    }

                    if (!found_node)
                    {
                        xmlAddChild(extension_activated_list_Node, extensionNode );
                    }
                    //end adds node

                    for (std::vector< ::fwRuntime::ConfigurationElement::sptr >::iterator iter_cfe_extension =
                             (*iter_extension)->begin();
                         iter_cfe_extension != (*iter_extension)->end();
                         ++iter_cfe_extension )
                    {
                        Convert::fromConfigurationElementToXml( (*iter_cfe_extension), extensionNode);
                    }
                }
            }//end Extensions parsing

            //cleaning : delete empty node (or if contain empty node)
            // xmlKeepBlanksDefault(0)  don't work; only for  TEXT NODES
            // xmlIsBlankNode too, work only for Text-Nodes
            if (!(extensionPoint_activated_list_Node->children))
            {
                xmlUnlinkNode(extensionPoint_activated_list_Node);
                xmlFreeNode(extensionPoint_activated_list_Node);
            }

            if (!(extension_activated_list_Node->children))
            {
                xmlUnlinkNode(extension_activated_list_Node);
                xmlFreeNode(extension_activated_list_Node);
            }

            if (!(bundleNode->children))
            {
                xmlUnlinkNode(bundleNode);
                xmlFreeNode(bundleNode);
            }
            //end cleaning
        }//end bundles iterator
    }
    while ( enable_Value );

    return node_root;
}//runningBundlesToXml

//------------------------------------------------------------------------------

xmlNodePtr Convert::toXml( ::fwRuntime::ConfigurationElement::sptr _cfgElement)
{
    xmlNodePtr tmp = xmlNewNode( NULL,  xmlCharStrdup( "Configurations_Elements" ) );
    ::fwRuntime::Convert::fromConfigurationElementToXml( _cfgElement, tmp);
    return tmp;
}

//------------------------------------------------------------------------------

std::string Convert::toXmlString( ::fwRuntime::ConfigurationElement::sptr _cfgElement)
{
    xmlNodePtr node     = toXml( _cfgElement );
    xmlBufferPtr buffer = xmlBufferCreate();
    xmlNodeDump( buffer, node->doc, xmlFirstElementChild(node), 0, 1 );

    std::string result = reinterpret_cast<const char*>(buffer->content);

    xmlFreeNode( node );
    xmlBufferFree( buffer );
    return result;
}

//------------------------------------------------------------------------------

::boost::property_tree::ptree Convert::toPropertyTree( ::fwRuntime::ConfigurationElement::csptr _cfgElement)
{
    ::boost::property_tree::ptree pt;
    ::boost::property_tree::ptree ptAttr;

    std::string propertyName  = _cfgElement->getName();
    std::string propertyValue = _cfgElement->getValue();

    typedef std::map<std::string, std::string> AttributeMatType;

    if(!propertyValue.empty())
    {
        pt.put(propertyName, propertyValue);
    }

    AttributeMatType attr = _cfgElement->getAttributes();

    for ( AttributeMatType::iterator iter = attr.begin(); iter != attr.end(); ++iter)
    {
        ptAttr.put(iter->first, iter->second);
    }

    if(!ptAttr.empty())
    {
        pt.put_child(propertyName + ".<xmlattr>", ptAttr);
    }

    for ( auto iterElement : _cfgElement->getElements() )
    {
        const std::string childName = iterElement->getName();
        ::boost::property_tree::ptree ptChild;

        ptChild = toPropertyTree(iterElement);

        boost::optional< ::boost::property_tree::ptree& > child = ptChild.get_child_optional(childName);

        if(child)
        {
            ptChild = *child;
        }

        pt.add_child( propertyName + "." + childName, ptChild );
    }
    return pt;
}

//------------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr Convert::fromPropertyTree( ::boost::property_tree::ptree pt )
{
    std::stringstream sstr;
    ::boost::property_tree::write_xml(sstr, pt);

    std::string xml = sstr.str();

    xmlDocPtr doc = xmlParseMemory(xml.c_str(), static_cast<int>(xml.size()));

    if (doc == NULL)
    {
        return ::fwRuntime::ConfigurationElement::sptr();
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);

    ::fwRuntime::ConfigurationElement::sptr ce;
    ce = ::fwRuntime::io::BundleDescriptorReader::processConfigurationElement(root, SPTR(Bundle)());

    xmlFreeDoc(doc);

    return ce;
}

//------------------------------------------------------------------------------

}//namespace fwRuntime
