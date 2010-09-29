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

//Convert *  Convert::m_singleton ;


Convert::Convert() {}

Convert::~Convert() {}

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
        if ((iter_attr_cfe->first)==std::string("class"))
        {
            xmlSetProp(pNode , xmlCharStrdup((iter_attr_cfe->first).c_str()), xmlCharStrdup((iter_attr_cfe->second).c_str()) ) ;
        }
    }

    for ( std::vector< ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > >::iterator iter_cfeC = _cfgElement->begin() ;
    iter_cfeC != _cfgElement->end() ;
    ++iter_cfeC )
    {
        //CHILDS
        //pour debugger ajouter le noeud purement textuel "configuration_element" ici
        //ainsi on ne confondera pas avec les attributs.
        /*xmlNodePtr tmp = xmlNewNode( NULL,  xmlCharStrdup( "Configurations_Elements_FILS..." ) );
        xmlAddChild(_node, tmp ) ;
        fromConfigurationElementToXml( (*iter_cfeC),  tmp) ; */

        fromConfigurationElementToXml( (*iter_cfeC), pNode) ;
    }
}

/*
//add properties of ConfigurationElement in the node given.
void addCfePropertiesToNode( ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement, xmlNodePtr _node)
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
    }

    for ( std::vector< ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > >::iterator iter_cfeC = _cfgElement->begin() ;
    iter_cfeC != _cfgElement->end() ;
    ++iter_cfeC )
    {
        //CHILDS
        addCfePropertiesToNode( (*iter_cfeC), pNode) ;
    }
}
*/

xmlNodePtr Convert::runningBundlesToXml( )
{
    xmlNodePtr node_root = xmlNewNode( NULL,  xmlCharStrdup( "Bundles" ) ) ;
    std::set< ::boost::shared_ptr< ::fwRuntime::Bundle > > ::iterator iter_bundles ;
    ::fwRuntime::Runtime * tmp_runtime = ::fwRuntime::Runtime::getDefault() ;

    xmlNodePtr activated_Node = xmlNewNode( NULL,  xmlCharStrdup( "Activated" ) ) ;
    xmlAddChild(node_root, activated_Node ) ;

    xmlNodePtr inactivated_Node = xmlNewNode( NULL,  xmlCharStrdup( "Inactivated" ) ) ;
    xmlAddChild(node_root, inactivated_Node ) ;


    bool Enable_Value = false ; // la boucle do ...  while() est parcourue deux fois, elle s'arr�te quand Enable_Value=False.
    do
    {
    Enable_Value = !Enable_Value ;
    for (iter_bundles =  tmp_runtime->bundlesBegin() ;
        iter_bundles != tmp_runtime->bundlesEnd() ;
        ++iter_bundles)
    {
        //BUNDLE

        xmlNodePtr bundleNode = xmlNewNode( NULL, xmlCharStrdup( (*iter_bundles)->getIdentifier().c_str() ) ) ;
        if (Enable_Value)
        {xmlAddChild(activated_Node, bundleNode ) ;}
        else
        {xmlAddChild(inactivated_Node, bundleNode ) ;}

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
            if (((*iter_extensionPoints)->isEnable())==Enable_Value)
                { xmlAddChild(extensionPoint_activated_list_Node, extensionPointsNode ) ;  }

            std::vector< ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > > AllConfigElement = ((*iter_extensionPoints)->getAllConfigurationElements());
            for (std::vector< ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > >::const_iterator iter_config_elem = AllConfigElement.begin() ;
                iter_config_elem != AllConfigElement.end() ;
                ++iter_config_elem)
            {
                //CONFIGURATIONS ELEMENTS
                //xmlSetProp(extensionPointsNode , xmlCharStrdup((iter_attr_cfe->first).c_str()), xmlCharStrdup((iter_attr_cfe->second).c_str()) ) ;
                //xmlAddChild(extensionPointsNode, ::fwRuntime::Convert::toXml((*iter_config_elem) ) ) ; fromConfigurationElementToXml( (*iter_config_elem), tmp)

                //xmlAddChild( extensionPointsNode, ::fwRuntime::Convert::toXml( (*iter_config_elem) ) ) ;

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
            if (((*iter_extension)->isEnable())==Enable_Value)
                {
                    std::string str = (*iter_extension)->getPoint() ;
                    //std::string str = (*iter_extension)->getIdentifier() ; // inapropri�, car des fois vide.
                    //xmlNodePtr extensionNode = xmlNewNode( NULL,  xmlCharStrdup( (str.substr(str.find_last_of("::")+1)).c_str() ) ) ;
                    xmlNodePtr extensionNode = xmlNewNode( NULL,  xmlCharStrdup( (str.c_str() ) ) ) ;

                    //Ajout du noeud si non existant
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
                        else
                        {

                            /*xmlAddChild( node , extensionNode ) ;
                            found_node = true ;
                            break ;*/
                        }
                    }

                    if (!found_node)
                    {
                        xmlAddChild(extension_activated_list_Node, extensionNode ) ;
                    }
                    //end ajout du noeud

                    for (std::vector< ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > >::iterator iter_cfe_extension = (*iter_extension)->begin() ;
                        iter_cfe_extension != (*iter_extension)->end() ;
                        ++iter_cfe_extension )
                    {
                        //xmlAddChild(extensionNode, ::fwRuntime::Convert::toXml((*iter_cfe_extension) ) ) ;

                        Convert::fromConfigurationElementToXml( (*iter_cfe_extension), extensionNode) ;

                    }
                }
        }//end Extensions parsing

        //cleaning : delete empty node (or if contain empty node)
        // xmlKeepBlanksDefault(0)  don't work; only for  TEXT NODES
        // xmlIsBlankNode too, work only for Text-Nodes
        if (!(extensionPoint_activated_list_Node->children))
            {xmlUnlinkNode(extensionPoint_activated_list_Node) ;
            xmlFreeNode(extensionPoint_activated_list_Node) ;}

        if (!(extension_activated_list_Node->children))
            {xmlUnlinkNode(extension_activated_list_Node) ;
            xmlFreeNode(extension_activated_list_Node) ;}

        if (!(bundleNode->children))
        {xmlUnlinkNode(bundleNode) ;
        xmlFreeNode(bundleNode) ;}
        //end cleaning
    }//boucle for, parcours des bundles
    } while ( Enable_Value ) ;

    return node_root ;
}//runningBundlesToXml

xmlNodePtr Convert::toXml( ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement)
{
    xmlNodePtr tmp = xmlNewNode( NULL,  xmlCharStrdup( "Configurations_Elements" ) );
    fromConfigurationElementToXml( _cfgElement, tmp) ;
    return tmp ;
}


}//namespace fwRuntime
