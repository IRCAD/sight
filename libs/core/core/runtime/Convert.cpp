/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/runtime/Convert.hpp"

#include "core/runtime/detail/ExtensionPoint.hpp"
#include "core/runtime/detail/io/ModuleDescriptorReader.hpp"
#include "core/runtime/detail/Module.hpp"
#include "core/runtime/detail/Runtime.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <set>

namespace sight::core::runtime::Convert
{

//------------------------------------------------------------------------------

void fromConfigurationElementToXml(
    std::shared_ptr<core::runtime::ConfigurationElement> _cfgElement,
    xmlNodePtr _node
)
{
    //NAME
    xmlNodePtr pNode = xmlNewNode(nullptr, xmlCharStrdup(_cfgElement->getName().c_str()));
    xmlAddChild(_node, pNode);

    std::string nodeValue = _cfgElement->getValue();
    if(!nodeValue.empty())
    {
        xmlNodeSetContent(pNode, reinterpret_cast<const xmlChar*>(nodeValue.c_str()));
    }

    std::map<std::string, std::string> attr_cfe = _cfgElement->getAttributes();

    for(auto& iter_attr_cfe : attr_cfe)
    {
        //ATTRIBUTES + VALUES
        xmlSetProp(
            pNode,
            xmlCharStrdup((iter_attr_cfe.first).c_str()),
            xmlCharStrdup((iter_attr_cfe.second).c_str())
        );
        if((iter_attr_cfe.first) == std::string("class"))
        {
            xmlSetProp(
                pNode,
                xmlCharStrdup((iter_attr_cfe.first).c_str()),
                xmlCharStrdup((iter_attr_cfe.second).c_str())
            );
        }
    }

    SIGHT_ASSERT(
        "ConfigurationElement should not have children("
        << _cfgElement->size() << ") and a value ("
        << nodeValue << ") at the same time.",
        !(!nodeValue.empty() && _cfgElement->size())
    );

    for(auto& iter_cfeC : *_cfgElement)
    {
        fromConfigurationElementToXml(iter_cfeC, pNode);
    }
}

//------------------------------------------------------------------------------

xmlNodePtr runningModulesToXml()
{
    xmlNodePtr node_root      = xmlNewNode(nullptr, xmlCharStrdup(MODULE_RC_PREFIX));
    xmlNodePtr activated_Node = xmlNewNode(nullptr, xmlCharStrdup("Activated"));
    xmlAddChild(node_root, activated_Node);

    xmlNodePtr inactivated_Node = xmlNewNode(nullptr, xmlCharStrdup("Inactivated"));
    xmlAddChild(node_root, inactivated_Node);

    bool enable_Value        = false; // the 'do while' loop stop if enable_Value==false.
    detail::Runtime& runtime = detail::Runtime::get();
    do
    {
        enable_Value = !enable_Value;
        for(const auto& module : runtime.getModules())
        {
            //MODULE
            xmlNodePtr moduleNode = xmlNewNode(nullptr, xmlCharStrdup(module->getIdentifier().c_str()));
            if(enable_Value)
            {
                xmlAddChild(activated_Node, moduleNode);
            }
            else
            {
                xmlAddChild(inactivated_Node, moduleNode);
            }

            //EXTENSIONS POINTS
            xmlNodePtr extensionPoint_activated_list_Node = xmlNewNode(nullptr, xmlCharStrdup("Extensions_Points"));
            xmlAddChild(moduleNode, extensionPoint_activated_list_Node);

            const auto moduleImpl = std::dynamic_pointer_cast<core::runtime::detail::Module>(module);
            for(auto
                iter_extensionPoints
                    =
                        moduleImpl->extensionPointsBegin() ;
                iter_extensionPoints != moduleImpl->extensionPointsEnd() ;
                ++iter_extensionPoints)
            {
                //EXTENSIONS POINTS
                std::string str = "Identifier : " + (*iter_extensionPoints)->getIdentifier();
                //-----DEBUG------
                xmlNodePtr extensionPointsNode = xmlNewNode(nullptr, xmlCharStrdup(str.c_str()));
                //xmlNodePtr extensionPointsNode = xmlNewNode( NULL,  xmlCharStrdup(
                // (str.substr(str.find_last_of("::")+1)).c_str() ) ) ;
                //-----DEBUG------
                if(((*iter_extensionPoints)->isEnabled()) == enable_Value)
                {
                    xmlAddChild(extensionPoint_activated_list_Node, extensionPointsNode);
                }

                std::vector<core::runtime::ConfigurationElement::sptr> AllConfigElement =
                    ((*iter_extensionPoints)->getAllConfigurationElements());
                for(auto& iter_config_elem : AllConfigElement)
                {
                    //CONFIGURATIONS ELEMENTS
                    Convert::fromConfigurationElementToXml(iter_config_elem, extensionPointsNode);
                }
            } //end extensionsPoints parsing

            //Extensions
            xmlNodePtr extension_activated_list_Node = xmlNewNode(nullptr, xmlCharStrdup("Extensions"));
            xmlAddChild(moduleNode, extension_activated_list_Node);

            for(auto iter_extension =
                    moduleImpl->extensionsBegin() ;
                iter_extension != moduleImpl->extensionsEnd() ;
                ++iter_extension)
            {
                if(((*iter_extension)->isEnabled()) == enable_Value)
                {
                    std::string str          = (*iter_extension)->getPoint();
                    xmlNodePtr extensionNode = xmlNewNode(nullptr, xmlCharStrdup((str.c_str())));

                    // Adds node if not exist
                    xmlNodePtr node = nullptr;
                    bool found_node = false;
                    for(node = extension_activated_list_Node->children ;
                        node != nullptr ;
                        node = node->next)
                    {
                        if((node->type == XML_ELEMENT_NODE)
                           && ((xmlStrcmp(node->name, xmlCharStrdup((str.c_str())))) == 0))
                        {
                            extensionNode = node;
                            found_node    = true;
                            break;
                        }
                    }

                    if(!found_node)
                    {
                        xmlAddChild(extension_activated_list_Node, extensionNode);
                    }

                    //end adds node

                    for(auto& iter_cfe_extension : *(*iter_extension))
                    {
                        Convert::fromConfigurationElementToXml(iter_cfe_extension, extensionNode);
                    }
                }
            } //end Extensions parsing

            //cleaning : delete empty node (or if contain empty node)
            // xmlKeepBlanksDefault(0)  don't work; only for  TEXT NODES
            // xmlIsBlankNode too, work only for Text-Nodes
            if((extensionPoint_activated_list_Node->children) == nullptr)
            {
                xmlUnlinkNode(extensionPoint_activated_list_Node);
                xmlFreeNode(extensionPoint_activated_list_Node);
            }

            if((extension_activated_list_Node->children) == nullptr)
            {
                xmlUnlinkNode(extension_activated_list_Node);
                xmlFreeNode(extension_activated_list_Node);
            }

            if((moduleNode->children) == nullptr)
            {
                xmlUnlinkNode(moduleNode);
                xmlFreeNode(moduleNode);
            }

            //end cleaning
        } //end modules iterator
    }
    while(enable_Value);

    return node_root;
} //runningModulesToXml

//------------------------------------------------------------------------------

xmlNodePtr toXml(core::runtime::ConfigurationElement::sptr _cfgElement)
{
    xmlNodePtr tmp = xmlNewNode(nullptr, xmlCharStrdup("Configurations_Elements"));
    core::runtime::Convert::fromConfigurationElementToXml(_cfgElement, tmp);
    return tmp;
}

//------------------------------------------------------------------------------

std::string toXmlString(core::runtime::ConfigurationElement::sptr _cfgElement)
{
    xmlNodePtr node     = toXml(_cfgElement);
    xmlBufferPtr buffer = xmlBufferCreate();
    xmlNodeDump(buffer, node->doc, xmlFirstElementChild(node), 0, 1);

    std::string result = reinterpret_cast<const char*>(buffer->content);

    xmlFreeNode(node);
    xmlBufferFree(buffer);
    return result;
}

//------------------------------------------------------------------------------

boost::property_tree::ptree toPropertyTree(core::runtime::ConfigurationElement::csptr _cfgElement)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ptree ptAttr;

    std::string propertyName  = _cfgElement->getName();
    std::string propertyValue = _cfgElement->getValue();

    using AttributeMatType = std::map<std::string, std::string>;

    if(!propertyValue.empty())
    {
        pt.put(propertyName, propertyValue);
    }

    AttributeMatType attr = _cfgElement->getAttributes();

    for(auto& iter : attr)
    {
        ptAttr.put(iter.first, iter.second);
    }

    if(!ptAttr.empty())
    {
        pt.put_child(propertyName + ".<xmlattr>", ptAttr);
    }

    for(const auto& iterElement : _cfgElement->getElements())
    {
        const std::string childName = iterElement->getName();
        boost::property_tree::ptree ptChild;

        ptChild = toPropertyTree(iterElement);

        boost::optional<boost::property_tree::ptree&> child = ptChild.get_child_optional(childName);

        if(child)
        {
            ptChild = *child;
        }

        pt.add_child(std::string(propertyName) + "." + childName, ptChild);
    }

    return pt;
}

//------------------------------------------------------------------------------

core::runtime::ConfigurationElement::sptr fromPropertyTree(boost::property_tree::ptree pt)
{
    std::stringstream sstr;
    boost::property_tree::write_xml(sstr, pt);

    std::string xml = sstr.str();

    xmlDocPtr doc = xmlParseMemory(xml.c_str(), static_cast<int>(xml.size()));

    if(doc == nullptr)
    {
        return {};
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);

    core::runtime::ConfigurationElement::sptr ce;
    ce = core::runtime::detail::io::ModuleDescriptorReader::processConfigurationElement(root, SPTR(detail::Module)());

    xmlFreeDoc(doc);

    return ce;
}

//------------------------------------------------------------------------------

} //namespace sight::core::runtime::Convert
