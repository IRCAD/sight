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

#include "fwRuntime/io/BundleDescriptorReader.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/dl/Library.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/ExtensionPoint.hpp"
#include "fwRuntime/io/Validator.hpp"
#include "fwRuntime/operations.hpp"

#include <filesystem>

#include <libxml/parser.h>
#include <libxml/xinclude.h>

#include <iostream>
#include <sstream>
#include <string>
namespace fwRuntime
{

namespace io
{

std::string BundleDescriptorReader::CLASS("class");
std::string BundleDescriptorReader::EXTENSION("extension");
std::string BundleDescriptorReader::EXTENSION_POINT("extension-point");
std::string BundleDescriptorReader::ID("id");
std::string BundleDescriptorReader::IMPLEMENTS("implements");
std::string BundleDescriptorReader::LIBRARY("library");
std::string BundleDescriptorReader::NAME("name");
std::string BundleDescriptorReader::PLUGIN("plugin");
std::string BundleDescriptorReader::REQUIREMENT("requirement");
std::string BundleDescriptorReader::SCHEMA("schema");
std::string BundleDescriptorReader::VERSION("version");
std::string BundleDescriptorReader::POINT("point");

//------------------------------------------------------------------------------

const BundleDescriptorReader::BundleContainer BundleDescriptorReader::createBundles(
    const std::filesystem::path& location)
{
    // Normalizes the path.
    std::filesystem::path normalizedPath(std::filesystem::weakly_canonical(location));

    // Asserts that the repository is a valid directory path.
    if(std::filesystem::exists(normalizedPath) == false ||
       std::filesystem::is_directory(normalizedPath) == false)
    {
        throw RuntimeException("'" + normalizedPath.string() + "': not a directory.");
    }

    // Walk through the repository entries.
    BundleContainer bundles;
    std::filesystem::directory_iterator currentEntry(normalizedPath);
    std::filesystem::directory_iterator endEntry;
    for(; currentEntry != endEntry; ++currentEntry)
    {
        std::filesystem::path entryPath = *currentEntry;

        if(std::filesystem::is_directory(entryPath))
        {
            try
            {
                SPTR( ::fwRuntime::Bundle ) bundle = BundleDescriptorReader::createBundle(entryPath);
                if(bundle)
                {
                    bundles.push_back( bundle );
                }
            }
            catch(const RuntimeException& runtimeException)
            {
#if !SLM_INFO_ENABLED
                FwCoreNotUsedMacro(runtimeException);
#endif
                OSLM_INFO( "'"<< entryPath.string() << "': skipped. " << runtimeException.what() );
            }
            catch(const ::fwCore::Exception& exception)
            {
#if !SLM_INFO_ENABLED
                FwCoreNotUsedMacro(exception);
#endif
                OSLM_INFO( "'"<< entryPath.string() << "': skipped. " << exception.what() );
            }
        }
    }
    return bundles;
}

//------------------------------------------------------------------------------

std::shared_ptr<Bundle> BundleDescriptorReader::createBundle(const std::filesystem::path& location)
{
    std::shared_ptr<Bundle> bundle;

    std::filesystem::path descriptorLocation(location / "plugin.xml");
    if(std::filesystem::exists(descriptorLocation) == false)
    {
        throw ::fwCore::Exception(std::string("'plugin.xml': file not found in ") + location.string());
    }

    // Validation
    auto pluginXSDLocation = ::fwRuntime::getLibraryResourceFilePath("fwRuntime-" FWRUNTIME_VER "/plugin.xsd");

    Validator validator(pluginXSDLocation);
    if( validator.validate(descriptorLocation) == false )
    {
        throw RuntimeException("Invalid bundle descriptor file. " + validator.getErrorLog());
    }

    // Get the document.
    xmlDocPtr document = xmlParseFile( descriptorLocation.string().c_str() );
    if(document == 0)
    {
        throw RuntimeException("Unable to read the bundle descriptor file.");
    }

    try
    {
        // Get the root node.
        xmlNodePtr rootNode = xmlDocGetRootElement(document);

        if (xmlXIncludeProcessTreeFlags(rootNode, XML_PARSE_NOBASEFIX) == -1)
        {
            throw RuntimeException("Unable to manage xinclude !");
        }

        if(xmlStrcmp(rootNode->name, (const xmlChar*) PLUGIN.c_str()) != 0)
        {
            throw RuntimeException("Unexpected XML element");
        }

        // Creates and process the plugin element.
        // Get the descriptor location.
        std::filesystem::path completeLocation(location);

        if(!completeLocation.is_absolute())
        {
            completeLocation = ::fwRuntime::Runtime::getDefault()->getWorkingPath() / location;
        }

        bundle = processPlugin(rootNode, completeLocation);

        // Job's done!
        xmlFreeDoc(document);
    }
    catch(std::exception& exception)
    {
        xmlFreeDoc(document);
        throw exception;
    }
    return bundle;
}

//-----------------------------------------------------------------------------

ConfigurationElement::sptr BundleDescriptorReader::processConfigurationElement(xmlNodePtr node,
                                                                               const std::shared_ptr<Bundle> bundle)

{
    // Creates the configuration element.
    const std::string nodeName((const char*) node->name);
    ConfigurationElement::sptr configurationElement(new ConfigurationElement(bundle, nodeName));

    // Processes all attributes.
    xmlAttrPtr curAttr;
    for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
    {
        const std::string attrName((const char*) curAttr->name);
        const std::string value((const char*) curAttr->children->content);

        configurationElement->setAttributeValue(attrName, value);
    }

    // Process child nodes.
    xmlNodePtr curChild = node->children;
    for(curChild = node->children; curChild != 0; curChild = curChild->next)
    {
        if(curChild->type == XML_TEXT_NODE && !xmlIsBlankNode(curChild))
        {
            std::string value((const char*) curChild->content);
            // Even whitespace (non XML_TEXT_NODE) are considered as valid XML_TEXT_NODE
            OSLM_WARN_IF(
                "Bundle : " << ( bundle ? bundle->getIdentifier() : "<None>" ) << ", node: " << nodeName << ", blanks in xml nodes can result in unexpected behaviour. Consider using <![CDATA[ ... ]]>.",
                (value.find("\n") != std::string::npos || value.find("\t") != std::string::npos));

            configurationElement->setValue( configurationElement->getValue() + value );
            continue;
        }
        else if(curChild->type == XML_CDATA_SECTION_NODE )
        {
            const std::string value((const char*) curChild->content);
            configurationElement->setValue( configurationElement->getValue() + value );
            continue;
        }

        else if(curChild->type == XML_ELEMENT_NODE)
        {
            ConfigurationElement::sptr element(processConfigurationElement(curChild, bundle));
            configurationElement->addConfigurationElement(element);
            continue;
        }
    }

    // Job's done.
    return configurationElement;
}

//------------------------------------------------------------------------------

std::shared_ptr<Extension> BundleDescriptorReader::processExtension(xmlNodePtr node,
                                                                    const std::shared_ptr<Bundle> bundle)
{
    // Processes all extension attributes.
    xmlAttrPtr curAttr;
    std::string point;
    std::string identifier;
    for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
        {
            identifier = (const char*) curAttr->children->content;
            continue;
        }

        if(xmlStrcmp(curAttr->name, (const xmlChar*) IMPLEMENTS.c_str()) == 0)
        {
            point = (const char*) curAttr->children->content;
            continue;
        }
    }

    // Creates the extension instance.
    std::shared_ptr<Extension> extension(new Extension(bundle, identifier, point, node));

    // Processes child nodes which are configuration elements.
    xmlNodePtr curChild;
    for(curChild = node->children; curChild != 0; curChild = curChild->next)
    {
        if(curChild->type == XML_ELEMENT_NODE)
        {
            ConfigurationElement::sptr element(processConfigurationElement(curChild, bundle));
            extension->addConfigurationElement(element);
        }
    }

    // Job's done.
    return extension;
}

//------------------------------------------------------------------------------

BundleDescriptorReader::PointExtensionsPairType BundleDescriptorReader::processPoint(xmlNodePtr node,
                                                                                     const std::shared_ptr<Bundle> bundle)
{
    // Creates the extension instance.
    xmlAttrPtr curAttr;
    std::string schema;
    std::string identifier;
    for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
        {
            identifier = (const char*) curAttr->children->content;
            continue;
        }

        if(xmlStrcmp(curAttr->name, (const xmlChar*) SCHEMA.c_str()) == 0)
        {
            schema = (const char*) curAttr->children->content;
            continue;
        }
    }
    std::shared_ptr<ExtensionPoint> extensionPoint(new ExtensionPoint(bundle, identifier, schema));

    // Processes child nodes which declare identifier as extensions.
    std::vector< std::shared_ptr<Extension> > extensionContainer;
    xmlNodePtr curChild;
    for(curChild = node->children; curChild != 0; curChild = curChild->next)
    {
        if(curChild->type == XML_ELEMENT_NODE)
        {
            if( xmlStrcmp(curChild->name, (const xmlChar*) IMPLEMENTS.c_str()) == 0 )
            {
                std::string extensionId = (const char*) curChild->children->content;
                std::shared_ptr<Extension> extension(new Extension(bundle, identifier, extensionId, curChild));
                extensionContainer.push_back( extension );
            }
        }
    }

    return PointExtensionsPairType( extensionPoint, extensionContainer );
}

//------------------------------------------------------------------------------

std::shared_ptr<ExtensionPoint> BundleDescriptorReader::processExtensionPoint(xmlNodePtr node,
                                                                              const std::shared_ptr<Bundle> bundle)

{
    // Processes all extension attributes.
    xmlAttrPtr curAttr;
    std::string identifier;
    std::string schema;
    for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
        {
            identifier = (const char*) curAttr->children->content;
            continue;
        }

        if(xmlStrcmp(curAttr->name, (const xmlChar*) SCHEMA.c_str()) == 0)
        {
            schema = (const char*) curAttr->children->content;
            continue;
        }
    }
    // Creates the extension instance.
    std::shared_ptr<ExtensionPoint> point(new ExtensionPoint(bundle, identifier, schema));

    // Job's done.
    return point;
}

//------------------------------------------------------------------------------

std::shared_ptr<dl::Library> BundleDescriptorReader::processLibrary(xmlNodePtr node)
{
    // Processes all plugin attributes.
    xmlAttrPtr curAttr;
    std::string name;
    for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, (const xmlChar*) NAME.c_str()) == 0)
        {
            name = (const char*) curAttr->children->content;
            continue;
        }
    }

    // Creates the library
    std::shared_ptr<dl::Library> library( new dl::Library(name) );
    return library;
}

//------------------------------------------------------------------------------

std::shared_ptr<Bundle> BundleDescriptorReader::processPlugin(xmlNodePtr node,
                                                              const std::filesystem::path& location)
{
    // Creates the bundle.
    std::shared_ptr<Bundle> bundle;
    // Processes all plugin attributes.
    xmlAttrPtr curAttr;
    std::string bundleIdentifier;
    std::string version;
    std::string pluginClass;
    for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
        {
            bundleIdentifier = (const char*) curAttr->children->content;
            continue;
        }

        if(xmlStrcmp(curAttr->name, (const xmlChar*) CLASS.c_str()) == 0)
        {
            pluginClass = (const char*) curAttr->children->content;
            continue;
        }

        if(xmlStrcmp(curAttr->name, (const xmlChar*) VERSION.c_str()) == 0)
        {
            version = (const char*) curAttr->children->content;
            continue;
        }
    }
    SLM_ASSERT("bundle identifier is empty", !bundleIdentifier.empty());

    if( ::fwRuntime::Runtime::getDefault()->findBundle(bundleIdentifier, Version(version)))
    {
        return bundle;
    }
    if(pluginClass.empty() == true)
    {
        bundle = std::shared_ptr<Bundle>( new Bundle(location, bundleIdentifier, version) );
    }
    else
    {
        bundle = std::shared_ptr<Bundle>( new Bundle(location, bundleIdentifier, version, pluginClass) );
    }

    // Processes all child nodes.
    xmlNodePtr curChild;
    for(curChild = node->children; curChild != 0; curChild = curChild->next)
    {
        // Skip non element nodes.
        if(curChild->type != XML_ELEMENT_NODE)
        {
            continue;
        }

        // Extension declaration.
        if(xmlStrcmp(curChild->name, (const xmlChar*) EXTENSION.c_str()) == 0)
        {
            std::shared_ptr<Extension> extension(processExtension(curChild, bundle));
            bundle->addExtension(extension);
            continue;
        }

        // Extension point declaration.
        if(xmlStrcmp(curChild->name, (const xmlChar*) EXTENSION_POINT.c_str()) == 0)
        {
            std::shared_ptr<ExtensionPoint> point(processExtensionPoint(curChild, bundle));
            bundle->addExtensionPoint(point);
            continue;
        }

        // Library declaration.
        if(xmlStrcmp(curChild->name, (const xmlChar*) LIBRARY.c_str()) == 0)
        {
            std::shared_ptr<dl::Library> library(processLibrary(curChild));
            bundle->addLibrary(library);
            continue;
        }

        // Requirement declaration.
        if(xmlStrcmp(curChild->name, (const xmlChar*) REQUIREMENT.c_str()) == 0)
        {
            const std::string requirement(processRequirement(curChild));
            bundle->addRequirement(requirement);
        }

        // Point declaration.
        if(xmlStrcmp(curChild->name, (const xmlChar*) POINT.c_str()) == 0)
        {
            SLM_FATAL("This xml element  ( <point ... > </point> ) is deprecated (" + location.string() + ")" );
        }
    }

    // Job's done.
    return bundle;
}

//------------------------------------------------------------------------------

const std::string BundleDescriptorReader::processRequirement(xmlNodePtr node)
{
    // Processes all requirement attributes.
    xmlAttrPtr curAttr;
    std::string identifier;
    for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
        {
            identifier = (const char*) curAttr->children->content;
            continue;
        }
    }

    // Do some sanity checking.
    if(identifier.length() == 0)
    {
        throw RuntimeException("Invalid attribute.");
    }

    // Job's done
    return identifier;
}

//------------------------------------------------------------------------------

} // namesapce io
} // namespace fwRuntime
