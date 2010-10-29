/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/Convert.hpp>

#include <set>
#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/ExtensionPoint.hpp>


namespace fwRuntime
{

//------------------------------------------------------------------------------

Convert::Convert()
{}

//------------------------------------------------------------------------------

Convert::~Convert()
{}

//------------------------------------------------------------------------------

void Convert::fromConfigurationElementToXml( ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement, xmlNodePtr _node)
{
    //NAME
    xmlNodePtr pNode = xmlNewNode( NULL,  xmlCharStrdup( _cfgElement->getName().c_str() ) ) ;
    xmlAddChild(_node, pNode ) ;

    std::map<std::string, std::string> attr_cfe = _cfgElement->getAttributes() ;

    for ( std::map<std::string, std::string>::iterator iter_attr_cfe = attr_cfe.begin() ;
        iter_attr_cfe!= attr_cfe.end();
        ++iter_attr_cfe)
    {
        //ATTRIBUTES + VALUES
        xmlSetProp(pNode , xmlCharStrdup((iter_attr_cfe->first).c_str()), xmlCharStrdup((iter_attr_cfe->second).c_str()) ) ;
        if ((iter_attr_cfe->first) == std::string("class"))
        {
            xmlSetProp(pNode , xmlCharStrdup((iter_attr_cfe->first).c_str()), xmlCharStrdup((iter_attr_cfe->second).c_str()) ) ;
        }
    }

    for ( std::vector< ::fwRuntime::ConfigurationElement::sptr >::iterator iter_cfeC = _cfgElement->begin() ;
    iter_cfeC != _cfgElement->end() ;
    ++iter_cfeC )
    {
        fromConfigurationElementToXml( (*iter_cfeC), pNode) ;
    }
}

//------------------------------------------------------------------------------

xmlNodePtr Convert::runningBundlesToXml( )
{
    xmlNodePtr node_root = xmlNewNode( NULL,  xmlCharStrdup( "Bundles" ) ) ;
    std::set< ::boost::shared_ptr< ::fwRuntime::Bundle > > ::iterator iter_bundles ;
    ::fwRuntime::Runtime * tmp_runtime = ::fwRuntime::Runtime::getDefault() ;

    xmlNodePtr activated_Node = xmlNewNode( NULL,  xmlCharStrdup( "Activated" ) ) ;
    xmlAddChild(node_root, activated_Node ) ;

    xmlNodePtr inactivated_Node = xmlNewNode( NULL,  xmlCharStrdup( "Inactivated" ) ) ;
    xmlAddChild(node_root, inactivated_Node ) ;

    bool enable_Value = false ; // the 'do while' loop stop if enable_Value==false.
    do
    {
        enable_Value = !enable_Value ;
        for (iter_bundles =  tmp_runtime->bundlesBegin() ;
                iter_bundles != tmp_runtime->bundlesEnd() ;
                ++iter_bundles)
        {
            //BUNDLE

            xmlNodePtr bundleNode = xmlNewNode( NULL, xmlCharStrdup( (*iter_bundles)->getIdentifier().c_str() ) ) ;
            if (enable_Value)
            {
                xmlAddChild(activated_Node, bundleNode ) ;
            }
            else
            {
                xmlAddChild(inactivated_Node, bundleNode ) ;
            }

            //EXTENSIONS POINTS
            xmlNodePtr extensionPoint_activated_list_Node = xmlNewNode( NULL,  xmlCharStrdup( "Extensions_Points" ) ) ;
            xmlAddChild(bundleNode, extensionPoint_activated_list_Node ) ;

            for ( std::set< ::boost::shared_ptr< ::fwRuntime::ExtensionPoint > >::const_iterator iter_extensionPoints = (*iter_bundles)->extensionPointsBegin() ;
                    iter_extensionPoints != (*iter_bundles)->extensionPointsEnd() ;
                    ++iter_extensionPoints)
            {
                //EXTENSIONS POINTS
                std::string str = "Identifier : "+(*iter_extensionPoints)->getIdentifier() ;
                //-----DEBUG------
                xmlNodePtr extensionPointsNode = xmlNewNode( NULL,  xmlCharStrdup( str.c_str() ) ) ;
                //xmlNodePtr extensionPointsNode = xmlNewNode( NULL,  xmlCharStrdup( (str.substr(str.find_last_of("::")+1)).c_str() ) ) ;
                //-----DEBUG------
                if (((*iter_extensionPoints)->isEnable())==enable_Value)
                {
                    xmlAddChild(extensionPoint_activated_list_Node, extensionPointsNode ) ;
                }

                std::vector< ::fwRuntime::ConfigurationElement::sptr > AllConfigElement = ((*iter_extensionPoints)->getAllConfigurationElements());
                for (std::vector< ::fwRuntime::ConfigurationElement::sptr >::const_iterator iter_config_elem = AllConfigElement.begin() ;
                        iter_config_elem != AllConfigElement.end() ;
                        ++iter_config_elem)
                {
                    //CONFIGURATIONS ELEMENTS
                    Convert::fromConfigurationElementToXml( (*iter_config_elem), extensionPointsNode) ;
                }
            }//end extensionsPoints parsing

            //Extensions
            xmlNodePtr extension_activated_list_Node = xmlNewNode( NULL,  xmlCharStrdup( "Extensions" ) ) ;
            xmlAddChild(bundleNode, extension_activated_list_Node ) ;

            for ( std::set< ::boost::shared_ptr< ::fwRuntime::Extension > >::const_iterator iter_extension = (*iter_bundles)->extensionsBegin()  ;
                    iter_extension != (*iter_bundles)->extensionsEnd() ;
                    ++iter_extension)
            {
                if (((*iter_extension)->isEnable())==enable_Value)
                {
                    std::string str = (*iter_extension)->getPoint() ;
                    xmlNodePtr extensionNode = xmlNewNode( NULL,  xmlCharStrdup( (str.c_str() ) ) ) ;

                    // Adds node if not exist
                    xmlNodePtr node ;
                    bool found_node = false ;
                    for ( node = extension_activated_list_Node->children ;
                            node ;
                            node = node->next)
                    {
                        if ( (node->type == XML_ELEMENT_NODE) && !(xmlStrcmp(node->name, xmlCharStrdup( (str.c_str() ) ) ) ) )
                        {
                            extensionNode = node ;
                            found_node = true ;
                            break ;
                        }
                    }

                    if (!found_node)
                    {
                        xmlAddChild(extension_activated_list_Node, extensionNode ) ;
                    }
                    //end adds node

                    for (std::vector< ::fwRuntime::ConfigurationElement::sptr >::iterator iter_cfe_extension = (*iter_extension)->begin() ;
                            iter_cfe_extension != (*iter_extension)->end() ;
                            ++iter_cfe_extension )
                    {
                        Convert::fromConfigurationElementToXml( (*iter_cfe_extension), extensionNode) ;
                    }
                }
            }//end Extensions parsing

            //cleaning : delete empty node (or if contain empty node)
            // xmlKeepBlanksDefault(0)  don't work; only for  TEXT NODES
            // xmlIsBlankNode too, work only for Text-Nodes
            if (!(extensionPoint_activated_list_Node->children))
            {
                xmlUnlinkNode(extensionPoint_activated_list_Node) ;
                xmlFreeNode(extensionPoint_activated_list_Node) ;
            }

            if (!(extension_activated_list_Node->children))
            {
                xmlUnlinkNode(extension_activated_list_Node) ;
                xmlFreeNode(extension_activated_list_Node) ;
            }

            if (!(bundleNode->children))
            {
                xmlUnlinkNode(bundleNode) ;
                xmlFreeNode(bundleNode) ;
            }
            //end cleaning
        }//boucle for, parcours des bundles
    } while ( enable_Value ) ;

    return node_root ;
}//runningBundlesToXml

//------------------------------------------------------------------------------

xmlNodePtr Convert::toXml( ::fwRuntime::ConfigurationElement::sptr _cfgElement)
{
    xmlNodePtr tmp = xmlNewNode( NULL,  xmlCharStrdup( "Configurations_Elements" ) );
    ::fwRuntime::Convert::fromConfigurationElementToXml( _cfgElement, tmp) ;
    return tmp ;
}

//------------------------------------------------------------------------------


}//namespace fwRuntime
