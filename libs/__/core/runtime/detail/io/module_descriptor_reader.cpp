/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "core/runtime/detail/io/module_descriptor_reader.hpp"

#include "core/runtime/detail/dl/library.hpp"
#include "core/runtime/detail/extension_point.hpp"
#include "core/runtime/detail/io/validator.hpp"
#include "core/runtime/detail/module.hpp"
#include "core/runtime/extension.hpp"
#include "core/runtime/path.hpp"
#include "core/runtime/runtime.hpp"

#include <core/runtime/helper.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/dll.hpp>

#include <libxml/parser.h>
#include <libxml/xinclude.h>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

namespace sight::core::runtime::detail::io
{

static const std::string EXTENSION("extension");
static const std::string EXTENSION_POINT("extension-point");
static const std::string ID("id");
static const std::string IMPLEMENTS("implements");
static const std::string LIBRARY("library");
static const std::string PLUGIN("plugin");
static const std::string PRIORITY("priority");
static const std::string REQUIREMENT("requirement");
static const std::string SCHEMA("schema");
static const std::string POINT("point");

//------------------------------------------------------------------------------

std::pair<std::filesystem::path, module_descriptor_reader::module_container> module_descriptor_reader::create_modules(
    const std::filesystem::path& _location
)
{
    std::filesystem::path normalized_path(_location);

    if(normalized_path.is_relative())
    {
        // Assume we are relative to the location of the executable path
        // Otherwise, weakly_canonical will make it relative to the current dir,
        // which forces us to `cd` in the correct directory in the launcher script
        const auto current_path = boost::dll::program_location().parent_path().string();
        normalized_path = current_path / normalized_path;
    }

    normalized_path = std::filesystem::weakly_canonical(normalized_path);

    // Asserts that the repository is a valid directory path.
    if(!std::filesystem::exists(normalized_path)
       || !std::filesystem::is_directory(normalized_path))
    {
        throw runtime_exception("'" + normalized_path.string() + "': not a directory.");
    }

    module_container modules;
    const auto load_module_fn =
        [&](const std::filesystem::path& _path)
        {
            try
            {
                SPTR(module) module = module_descriptor_reader::create_module(_path);
                if(module)
                {
                    modules.push_back(module);
                }
            }
            catch(const runtime_exception& runtime_exception)
            {
                SIGHT_DEBUG("'" << _path.string() << "': skipped. " << runtime_exception.what());
            }
            catch(const core::exception& exception)
            {
                SIGHT_DEBUG("'" << _path.string() << "': skipped. " << exception.what());
            }
        };

    // Walk through the repository entries.
    std::filesystem::directory_iterator current_entry(normalized_path);
    std::filesystem::directory_iterator end_entry;
    for( ; current_entry != end_entry ; ++current_entry)
    {
        const std::filesystem::path entry_path = *current_entry;

        if(std::filesystem::is_directory(entry_path))
        {
            load_module_fn(entry_path);
        }
    }

    // If nothing can be found in the subfolders, give a try with the current folder
    // This is sometimes used in unit-test to load a specific module instead of a set of modules
    if(modules.empty())
    {
        load_module_fn(normalized_path);
    }

    return {normalized_path, modules};
}

//------------------------------------------------------------------------------

std::shared_ptr<module> module_descriptor_reader::create_module(const std::filesystem::path& _location)
{
    std::shared_ptr<detail::module> module;

    std::filesystem::path descriptor_location(_location / "plugin.xml");
    if(!std::filesystem::exists(descriptor_location))
    {
        throw core::exception(std::string("'plugin.xml': file not found in ") + _location.string());
    }

    // Validation
    auto plugin_xsdlocation = core::runtime::get_library_resource_file_path("core/plugin.xsd");

    validator validator(plugin_xsdlocation);
    if(!validator.validate(descriptor_location))
    {
        throw runtime_exception("Invalid module descriptor file. " + validator.get_error_log());
    }

    // Get the document.
    xmlDocPtr document = xmlParseFile(descriptor_location.string().c_str());
    if(document == nullptr)
    {
        throw runtime_exception("Unable to read the module descriptor file.");
    }

    try
    {
        // Get the root node.
        xmlNodePtr root_node = xmlDocGetRootElement(document);

        if(xmlXIncludeProcessTreeFlags(root_node, XML_PARSE_NOBASEFIX) == -1)
        {
            throw runtime_exception("Unable to manage xinclude !");
        }

        if(xmlStrcmp(root_node->name, reinterpret_cast<const xmlChar*>(PLUGIN.c_str())) != 0)
        {
            throw runtime_exception("Unexpected XML element");
        }

        // Creates and process the plugin element.
        // Get the descriptor location.
        std::filesystem::path complete_location(_location);

        if(!complete_location.is_absolute())
        {
            complete_location = core::runtime::working_path() / _location;
        }

        module = process_plugin(root_node, complete_location);

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

void module_descriptor_reader::process_configuration(xmlNodePtr _node, core::runtime::config_t& _parent_config)
{
    // Creates the configuration element.
    const std::string node_name(reinterpret_cast<const char*>(_node->name));
    core::runtime::config_t config;

    // Processes all attributes.
    xmlAttrPtr cur_attr = nullptr;
    for(cur_attr = _node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        const std::string attr_name(reinterpret_cast<const char*>(cur_attr->name));
        const std::string value(reinterpret_cast<const char*>(cur_attr->children->content));
        config.add("<xmlattr>." + attr_name, value);
    }

    if(node_name == "service")
    {
        cur_attr = nullptr;
    }

    // Process child nodes.
    std::string value;
    core::runtime::config_t children;
    for(xmlNodePtr cur_child = _node->children ; cur_child != nullptr ; cur_child = cur_child->next)
    {
        if(cur_child->type == XML_TEXT_NODE && (xmlIsBlankNode(cur_child) == 0))
        {
            std::string content(reinterpret_cast<const char*>(cur_child->content));
            // Even whitespace (non XML_TEXT_NODE) are considered as valid XML_TEXT_NODE
            SIGHT_WARN_IF(
                "Node: " << node_name
                << ", blanks in xml nodes can result in unexpected behaviour. Consider using <![CDATA[ ... ]]>.",
                (content.find('\n') != std::string::npos || content.find('\t') != std::string::npos)
            );

            value += content;
            continue;
        }

        if(cur_child->type == XML_CDATA_SECTION_NODE)
        {
            value += std::string(reinterpret_cast<const char*>(cur_child->content));
            continue;
        }

        if(cur_child->type == XML_ELEMENT_NODE)
        {
            process_configuration(cur_child, config);
            continue;
        }
    }

    if(!children.empty())
    {
        config.add_child(node_name, children);
    }

    if(!value.empty())
    {
        config.put_value<std::string>(value);
    }

    _parent_config.add_child(node_name, config);
}

//------------------------------------------------------------------------------

std::shared_ptr<extension> module_descriptor_reader::process_extension(
    xmlNodePtr _node,
    const std::shared_ptr<module> _module
)
{
    // Processes all extension attributes.
    xmlAttrPtr cur_attr = nullptr;
    std::string point;
    std::string identifier;
    for(cur_attr = _node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }

        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(IMPLEMENTS.c_str())) == 0)
        {
            point = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }
    }

    if(point == "sight::app::extension::config")
    {
        cur_attr = nullptr;
    }

    // Creates the extension instance.
    std::shared_ptr<extension> ext = std::make_shared<extension>(_module, identifier, point, _node);

    // Processes child nodes which are configuration elements.
    xmlNodePtr cur_child = nullptr;
    core::runtime::config_t config;
    for(cur_child = _node->children ; cur_child != nullptr ; cur_child = cur_child->next)
    {
        if(cur_child->type == XML_ELEMENT_NODE)
        {
            process_configuration(cur_child, config);
        }
    }

    ext->set_config(config);
    // Job's done.
    return ext;
}

//------------------------------------------------------------------------------

module_descriptor_reader::point_extensions_pair_type module_descriptor_reader::process_point(
    xmlNodePtr _node,
    const std::shared_ptr<module> _module
)
{
    // Creates the extension instance.
    xmlAttrPtr cur_attr = nullptr;
    std::string schema;
    std::string identifier;
    for(cur_attr = _node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }

        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(SCHEMA.c_str())) == 0)
        {
            schema = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }
    }

    auto extension_pt = std::make_shared<extension_point>(_module, identifier, schema);

    // Processes child nodes which declare identifier as extensions.
    std::vector<std::shared_ptr<extension> > extension_container;
    xmlNodePtr cur_child = nullptr;
    for(cur_child = _node->children ; cur_child != nullptr ; cur_child = cur_child->next)
    {
        if(cur_child->type == XML_ELEMENT_NODE)
        {
            if(xmlStrcmp(cur_child->name, reinterpret_cast<const xmlChar*>(IMPLEMENTS.c_str())) == 0)
            {
                std::string extension_id = reinterpret_cast<const char*>(cur_child->children->content);
                auto ext                 = std::make_shared<extension>(_module, identifier, extension_id, cur_child);
                extension_container.push_back(ext);
            }
        }
    }

    return {extension_pt, extension_container};
}

//------------------------------------------------------------------------------

std::shared_ptr<extension_point> module_descriptor_reader::process_extension_point(
    xmlNodePtr _node,
    const std::shared_ptr<module> _module
)
{
    // Processes all extension attributes.
    xmlAttrPtr cur_attr = nullptr;
    std::string identifier;
    std::string schema;
    for(cur_attr = _node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }

        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(SCHEMA.c_str())) == 0)
        {
            schema = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }
    }

    // Creates the extension instance.
    std::shared_ptr<extension_point> point = std::make_shared<extension_point>(_module, identifier, schema);

    // Job's done.
    return point;
}

//------------------------------------------------------------------------------

std::shared_ptr<detail::module> module_descriptor_reader::process_plugin(
    xmlNodePtr _node,
    const std::filesystem::path& _location
)
{
    // Creates the module.
    std::shared_ptr<detail::module> module;
    // Processes all plugin attributes.
    xmlAttrPtr cur_attr = nullptr;
    std::string module_identifier;
    for(cur_attr = _node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            module_identifier = reinterpret_cast<const char*>(cur_attr->children->content);
            module_identifier = filter_id(module_identifier);
            continue;
        }
    }

    SIGHT_ASSERT("module identifier is empty", !module_identifier.empty());

    if(core::runtime::find_module(module_identifier))
    {
        return module;
    }

    bool create_library = false;
    int priority        = 0;
    for(cur_attr = _node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(LIBRARY.c_str())) == 0)
        {
            create_library = (xmlStrcmp(cur_attr->children->content, reinterpret_cast<const xmlChar*>("true")) == 0);
        }

        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(PRIORITY.c_str())) == 0)
        {
            priority = std::stoi(reinterpret_cast<const char*>(cur_attr->children->content));
        }
    }

    if(create_library)
    {
        // Deduce the library name from the plugin name
        std::string libname = boost::algorithm::replace_all_copy(module_identifier, "::", "_");
        boost::algorithm::trim_left_if(libname, [](auto _x){return _x == '_';});

        SIGHT_INFO(std::string("plugin ") + module_identifier + " holds library " + libname);

        // Creates the library
        // If we have a library, deduce the plugin name
        const std::string plugin_class = module_identifier + "::plugin";

        module = std::make_shared<detail::module>(_location, module_identifier, plugin_class, priority);

        auto library = std::make_shared<dl::library>(libname);
        module->set_library(library);
    }
    else
    {
        module = std::make_shared<detail::module>(_location, module_identifier);
    }

    // Processes all child nodes.
    xmlNodePtr cur_child = nullptr;
    for(cur_child = _node->children ; cur_child != nullptr ; cur_child = cur_child->next)
    {
        // Skip non element nodes.
        if(cur_child->type != XML_ELEMENT_NODE)
        {
            continue;
        }

        // Extension declaration.
        if(xmlStrcmp(cur_child->name, reinterpret_cast<const xmlChar*>(EXTENSION.c_str())) == 0)
        {
            std::shared_ptr<extension> extension(process_extension(cur_child, module));
            module->add_extension(extension);
            continue;
        }

        // Extension point declaration.
        if(xmlStrcmp(cur_child->name, reinterpret_cast<const xmlChar*>(EXTENSION_POINT.c_str())) == 0)
        {
            std::shared_ptr<extension_point> point(process_extension_point(cur_child, module));
            module->add_extension_point(point);
            continue;
        }

        // Requirement declaration.
        if(xmlStrcmp(cur_child->name, reinterpret_cast<const xmlChar*>(REQUIREMENT.c_str())) == 0)
        {
            const std::string requirement(process_requirement(cur_child));
            module->add_requirement(requirement);
        }

        // Point declaration.
        if(xmlStrcmp(cur_child->name, reinterpret_cast<const xmlChar*>(POINT.c_str())) == 0)
        {
            SIGHT_FATAL("This xml element  ( <point ... > </point> ) is deprecated (" + _location.string() + ")");
        }
    }

    // Job's done.
    return module;
}

//------------------------------------------------------------------------------

std::string module_descriptor_reader::process_requirement(xmlNodePtr _node)
{
    // Processes all requirement attributes.
    xmlAttrPtr cur_attr = nullptr;
    std::string identifier;
    for(cur_attr = _node->properties ; cur_attr != nullptr ; cur_attr = cur_attr->next)
    {
        if(xmlStrcmp(cur_attr->name, reinterpret_cast<const xmlChar*>(ID.c_str())) == 0)
        {
            identifier = reinterpret_cast<const char*>(cur_attr->children->content);
            continue;
        }
    }

    // Do some sanity checking.
    if(identifier.length() == 0)
    {
        throw runtime_exception("Invalid attribute.");
    }

    // Job's done
    return identifier;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::io
