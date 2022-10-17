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

#include "core/runtime/detail/io/ModuleDescriptorReader.hpp"

#include "core/runtime/ConfigurationElement.hpp"
#include "core/runtime/detail/dl/Library.hpp"
#include "core/runtime/detail/ExtensionPoint.hpp"
#include "core/runtime/detail/io/Validator.hpp"
#include "core/runtime/detail/Module.hpp"
#include "core/runtime/Extension.hpp"
#include "core/runtime/operations.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <libxml/parser.h>
#include <libxml/xinclude.h>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

namespace sight::core::runtime::detail::io
{

static const std::string CLASS("class");
static const std::string EXTENSION("extension");
static const std::string EXTENSION_POINT("extension-point");
static const std::string ID("id");
static const std::string IMPLEMENTS("implements");
static const std::string LIBRARY("library");
static const std::string NAME("name");
static const std::string PLUGIN("plugin");
static const std::string PRIORITY("priority");
static const std::string REQUIREMENT("requirement");
static const std::string SCHEMA("schema");
static const std::string POINT("point");

//------------------------------------------------------------------------------

ModuleDescriptorReader::ModuleContainer ModuleDescriptorReader::createModules(
    const std::filesystem::path& location
)
{
    // Normalizes the path.
    std::filesystem::path normalizedPath(std::filesystem::weakly_canonical(location));

    // Asserts that the repository is a valid directory path.
    if(!std::filesystem::exists(normalizedPath)
       || !std::filesystem::is_directory(normalizedPath))
    {
        throw RuntimeException("'" + normalizedPath.string() + "': not a directory.");
    }

    // Walk through the repository entries.
    ModuleContainer modules;
    std::filesystem::directory_iterator currentEntry(normalizedPath);
    std::filesystem::directory_iterator endEntry;
    for( ; currentEntry != endEntry ; ++currentEntry)
    {
        const std::filesystem::path entryPath = *currentEntry;

        if(std::filesystem::is_directory(entryPath))
        {
            try
            {
                SPTR(Module) module = ModuleDescriptorReader::createModule(entryPath);
                if(module)
                {
                    modules.push_back(module);
                }
            }
            catch(const RuntimeException& runtimeException)
            {
                SIGHT_DEBUG("'" << entryPath.string() << "': skipped. " << runtimeException.what());
            }
            catch(const core::Exception& exception)
            {
                SIGHT_DEBUG("'" << entryPath.string() << "': skipped. " << exception.what());
            }
        }
    }

    return modules;
}

//------------------------------------------------------------------------------

std::shared_ptr<Module> ModuleDescriptorReader::createModule(const std::filesystem::path& location)
{
    std::shared_ptr<detail::Module> module;

    std::filesystem::path descriptorLocation(location / "plugin.xml");
    if(!std::filesystem::exists(descriptorLocation))
    {
        throw core::Exception(std::string("'plugin.xml': file not found in ") + location.string());
    }

    // Validation
    auto pluginXSDLocation = core::runtime::getLibraryResourceFilePath("core/plugin.xsd");

    Validator validator(pluginXSDLocation);
    if(!validator.validate(descriptorLocation))
    {
        throw RuntimeException("Invalid module descriptor file. " + validator.getErrorLog());
    }

    // Get the document.
    xmlDocPtr document = xmlParseFile(descriptorLocation.string().c_str());
    if(document == nullptr)
    {
        throw RuntimeException("Unable to read the module descriptor file.");
    }

    try
    {
        // Get the root node.
        xmlNodePtr rootNode = xmlDocGetRootElement(document);

        if(xmlXIncludeProcessTreeFlags(rootNode, XML_PARSE_NOBASEFIX) == -1)
        {
            throw RuntimeException("Unable to manage xinclude !");
        }

        if(xmlStrcmp(rootNode->name, reinterpret_cast<const xmlChar*>(PLUGIN.c_str())) != 0)
        {
            throw RuntimeException("Unexpected XML element");
        }

        // Creates and process the plugin element.
        // Get the descriptor location.
        std::filesystem::path completeLocation(location);

        if(!completeLocation.is_absolute())
        {
            completeLocation = core::runtime::Runtime::getDefault()->getWorkingPath() / location;
        }

        module = processPlugin(rootNode, completeLocation);

        // Job's done!
        xmlFreeDoc(document);
    }
    catch(std::exception& /*exception*/)
    {
        xmlFreeDoc(document);
        throw;
    }
    return module;
}

//-----------------------------------------------------------------------------

ConfigurationElement::sptr ModuleDescriptorReader::processConfigurationElement(
    xmlNodePtr node,
    const std::shared_ptr<Module> module
)
{
    // Creates the configuration element.
    const std::string nodeName(reinterpret_cast<const char*>(node->name));
    ConfigurationElement::sptr configurationElement(ConfigurationElement::New(module, nodeName));

    // Processes all attributes.
    xmlAttrPtr curAttr = nullptr;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        const std::string attrName(reinterpret_cast<const char*>(curAttr->name));
        const std::string value(reinterpret_cast<const char*>(curAttr->children->content));

        configurationElement->setAttributeValue(attrName, value);
    }

    // Process child nodes.
    for(xmlNodePtr curChild = node->children ; curChild != nullptr ; curChild = curChild->next)
    {
        if(curChild->type == XML_TEXT_NODE && (xmlIsBlankNode(curChild) == 0))
        {
            std::string value(reinterpret_cast<const char*>(curChild->content));
            // Even whitespace (non XML_TEXT_NODE) are considered as valid XML_TEXT_NODE
            SIGHT_WARN_IF(
                "Module : " << (module ? module->getIdentifier() : "<None>") << ", node: " << nodeName << ", blanks in xml nodes can result in unexpected behaviour. Consider using <![CDATA[ ... ]]>.",
                (value.find('\n') != std::string::npos || value.find('\t') != std::string::npos)
            );

            configurationElement->setValue(configurationElement->getValue() + value);
            continue;
        }

        if(curChild->type == XML_CDATA_SECTION_NODE)
        {
            const std::string value(reinterpret_cast<const char*>(curChild->content));
            configurationElement->setValue(configurationElement->getValue() + value);
            continue;
        }

        if(curChild->type == XML_ELEMENT_NODE)
        {
            ConfigurationElement::sptr element(processConfigurationElement(curChild, module));
            configurationElement->addConfigurationElement(element);
            continue;
        }
    }

    // Job's done.
    return configurationElement;
}

//------------------------------------------------------------------------------

std::shared_ptr<Extension> ModuleDescriptorReader::processExtension(
    xmlNodePtr node,
    const std::shared_ptr<Module> module
)
{
    // Processes all extension attributes.
    xmlAttrPtr curAttr = nullptr;
    std::string point;
    std::string identifier;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }

        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(IMPLEMENTS.c_str())) == 0)
        {
            point = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }
    }

    // Creates the extension instance.
    std::shared_ptr<Extension> extension = std::make_shared<Extension>(module, identifier, point, node);

    // Processes child nodes which are configuration elements.
    xmlNodePtr curChild = nullptr;
    for(curChild = node->children ; curChild != nullptr ; curChild = curChild->next)
    {
        if(curChild->type == XML_ELEMENT_NODE)
        {
            ConfigurationElement::sptr element(processConfigurationElement(curChild, module));
            extension->addConfigurationElement(element);
        }
    }

    // Job's done.
    return extension;
}

//------------------------------------------------------------------------------

ModuleDescriptorReader::PointExtensionsPairType ModuleDescriptorReader::processPoint(
    xmlNodePtr node,
    const std::shared_ptr<Module> module
)
{
    // Creates the extension instance.
    xmlAttrPtr curAttr = nullptr;
    std::string schema;
    std::string identifier;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }

        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(SCHEMA.c_str())) == 0)
        {
            schema = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }
    }

    auto extensionPoint = std::make_shared<ExtensionPoint>(module, identifier, schema);

    // Processes child nodes which declare identifier as extensions.
    std::vector<std::shared_ptr<Extension> > extensionContainer;
    xmlNodePtr curChild = nullptr;
    for(curChild = node->children ; curChild != nullptr ; curChild = curChild->next)
    {
        if(curChild->type == XML_ELEMENT_NODE)
        {
            if(xmlStrcmp(curChild->name, reinterpret_cast<const xmlChar*>(IMPLEMENTS.c_str())) == 0)
            {
                std::string extensionId = reinterpret_cast<const char*>(curChild->children->content);
                auto extension          = std::make_shared<Extension>(module, identifier, extensionId, curChild);
                extensionContainer.push_back(extension);
            }
        }
    }

    return {extensionPoint, extensionContainer};
}

//------------------------------------------------------------------------------

std::shared_ptr<ExtensionPoint> ModuleDescriptorReader::processExtensionPoint(
    xmlNodePtr node,
    const std::shared_ptr<Module> module
)
{
    // Processes all extension attributes.
    xmlAttrPtr curAttr = nullptr;
    std::string identifier;
    std::string schema;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }

        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(SCHEMA.c_str())) == 0)
        {
            schema = reinterpret_cast<const char*>(curAttr->children->content);
            continue;
        }
    }

    // Creates the extension instance.
    std::shared_ptr<ExtensionPoint> point = std::make_shared<ExtensionPoint>(module, identifier, schema);

    // Job's done.
    return point;
}

//------------------------------------------------------------------------------

std::shared_ptr<detail::Module> ModuleDescriptorReader::processPlugin(
    xmlNodePtr node,
    const std::filesystem::path& location
)
{
    // Creates the module.
    std::shared_ptr<detail::Module> module;
    // Processes all plugin attributes.
    xmlAttrPtr curAttr = nullptr;
    std::string moduleIdentifier;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            moduleIdentifier = reinterpret_cast<const char*>(curAttr->children->content);
            moduleIdentifier = filterID(moduleIdentifier);
            continue;
        }
    }

    SIGHT_ASSERT("module identifier is empty", !moduleIdentifier.empty());

    if(core::runtime::Runtime::getDefault()->findModule(moduleIdentifier))
    {
        return module;
    }

    bool createLibrary = false;
    int priority       = 0;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(LIBRARY.c_str())) == 0)
        {
            createLibrary = (xmlStrcmp(curAttr->children->content, reinterpret_cast<const xmlChar*>("true")) == 0);
        }

        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(PRIORITY.c_str())) == 0)
        {
            priority = std::stoi(reinterpret_cast<const char*>(curAttr->children->content));
        }
    }

    if(createLibrary)
    {
        // Deduce the library name from the plugin name
        std::string libname = boost::algorithm::replace_all_copy(moduleIdentifier, "::", "_");
        boost::algorithm::trim_left_if(libname, [](auto x){return x == '_';});

        SIGHT_INFO(std::string("Plugin ") + moduleIdentifier + " holds library " + libname);

        // Creates the library
        // If we have a library, deduce the plugin name
        const std::string pluginClass = moduleIdentifier + "::Plugin";

        module = std::make_shared<Module>(location, moduleIdentifier, pluginClass, priority);

        auto library = std::make_shared<dl::Library>(libname);
        module->setLibrary(library);
    }
    else
    {
        module = std::make_shared<Module>(location, moduleIdentifier);
    }

    // Processes all child nodes.
    xmlNodePtr curChild = nullptr;
    for(curChild = node->children ; curChild != nullptr ; curChild = curChild->next)
    {
        // Skip non element nodes.
        if(curChild->type != XML_ELEMENT_NODE)
        {
            continue;
        }

        // Extension declaration.
        if(xmlStrcmp(curChild->name, reinterpret_cast<const xmlChar*>(EXTENSION.c_str())) == 0)
        {
            std::shared_ptr<Extension> extension(processExtension(curChild, module));
            module->addExtension(extension);
            continue;
        }

        // Extension point declaration.
        if(xmlStrcmp(curChild->name, reinterpret_cast<const xmlChar*>(EXTENSION_POINT.c_str())) == 0)
        {
            std::shared_ptr<ExtensionPoint> point(processExtensionPoint(curChild, module));
            module->addExtensionPoint(point);
            continue;
        }

        // Requirement declaration.
        if(xmlStrcmp(curChild->name, reinterpret_cast<const xmlChar*>(REQUIREMENT.c_str())) == 0)
        {
            const std::string requirement(processRequirement(curChild));
            module->addRequirement(requirement);
        }

        // Point declaration.
        if(xmlStrcmp(curChild->name, reinterpret_cast<const xmlChar*>(POINT.c_str())) == 0)
        {
            SIGHT_FATAL("This xml element  ( <point ... > </point> ) is deprecated (" + location.string() + ")");
        }
    }

    // Job's done.
    return module;
}

//------------------------------------------------------------------------------

std::string ModuleDescriptorReader::processRequirement(xmlNodePtr node)
{
    // Processes all requirement attributes.
    xmlAttrPtr curAttr = nullptr;
    std::string identifier;
    for(curAttr = node->properties ; curAttr != nullptr ; curAttr = curAttr->next)
    {
        if(xmlStrcmp(curAttr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(curAttr->children->content);
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

} // namespace sight::core::runtime::detail::io
