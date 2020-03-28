/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#pragma once

#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif
#include <string>
#include <vector>
#include <filesystem>
#include <libxml/parser.h>
#if defined(__GNUC__)
#pragma GCC visibility pop
#endif

#include "fwRuntime/impl/ExtensionPoint.hpp"
#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/impl/dl/Library.hpp"

namespace fwRuntime
{

class Module;
struct ConfigurationElement;
struct Extension;

namespace impl
{

class Module;
class ExtensionPoint;

namespace io
{

/**
 * @brief   Defines the module descriptor reader class.
 */
class ModuleDescriptorReader
{
public:
    typedef std::vector< std::shared_ptr<Module> >  ModuleContainer;
    /// Pair of created extension point associated with extensions
    typedef std::pair< SPTR(ExtensionPoint), std::vector< SPTR(Extension) > > PointExtensionsPairType;

    /**
     * @brief       Creates all modules that are found at the given location.
     *
     * @param[in]   location    a path to a directory containing modules
     *
     * @return      a container with all created modules
     */
    static const ModuleContainer createModules(const std::filesystem::path& location);

    /**
     * @brief       Look for a descriptor at the specified location,
     *              reads it and creates a module with it.
     *
     * @param[in]   location    a path to a directory that should contain a module description
     *
     * @return      a shared pointer to the created module
     */
    static std::shared_ptr<Module> createModule(const std::filesystem::path& location);

    /**
     * @brief   Processes a configuration element XML node.
     *
     * @param   node    a pointer to the XML node that represents a configuration element
     * @param   module  a pointer to the module the extension will be attached to
     *
     * @return  a pointer to the created configuration element
     */
    static std::shared_ptr<ConfigurationElement> processConfigurationElement(xmlNodePtr node,
                                                                             const std::shared_ptr<Module> module)
    ;

    /**
     * @brief   Processes an extension XML node.
     *
     * @param   node    a pointer to the XML node that represents an extension
     * @param   module  a pointer to the module the extension will be attached to
     *
     * @return  a pointer to the created extension
     */
    static std::shared_ptr<Extension> processExtension(xmlNodePtr node,
                                                       const std::shared_ptr<Module> module);

    /**
     * Processes a node that is a point
     *
     * @param   node    a pointer to the xml document node that represents a requirement
     * @param   module  a pointer to the module the extension will be attached to
     * @return  a pair with the created extension point and extensions
     */
    static PointExtensionsPairType processPoint(xmlNodePtr node,
                                                const std::shared_ptr<Module> module);

    /**
     * @brief   Processes an extension point XML node.
     *
     * @param   node    a pointer to the XML node that represents an extension point
     * @param   module  a pointer to the module declaring the extension point
     *
     * @return  a pointer to the created extension point
     */
    static std::shared_ptr<ExtensionPoint> processExtensionPoint(xmlNodePtr node,
                                                                 const std::shared_ptr<Module> module);

    /**
     * @brief   Processes a library XML node.
     *
     * @param   node    a pointer to the XML node that represents a library declaration
     *
     * @return  a pointer to the created library
     */
    static std::shared_ptr<dl::Library> processLibrary(xmlNodePtr node);

    /**
     * Processes a node that is a plugin element.
     *
     * @param   node        a pointer to the xml document node that represents a module
     * @param   location    a path to a directory containing the module
     *
     * @return  a pointer to the created module
     */
    static std::shared_ptr<impl::Module> processPlugin(xmlNodePtr node,
                                                       const std::filesystem::path& location);

    /**
     * Processes a node that is a requirement
     *
     * @param   node        a pointer to the xml document node that represents a requirement
     *
     * @return  a string containing the requirement's value
     */
    static const std::string processRequirement(xmlNodePtr node);

private:

    static std::string CLASS;       ///< Defines the class XML element name.
    static std::string EXTENSION;   ///< Defines the extension XML element name.
    static std::string EXTENSION_POINT; ///< Defines the extension-point XML element name.
    static std::string ID;          ///< Defines the id XML element name.
    static std::string IMPLEMENTS;  ///< Defines the implements XML element name.
    static std::string NAME;        ///< Defines the name XML element name.
    static std::string LIBRARY;     ///< Defines the library XML element name.
    static std::string PLUGIN;      ///< Defines the plugin XML element name.
    static std::string REQUIREMENT; ///< Defines the requirement XML element name.
    static std::string SCHEMA;      ///< Defines the schema XML element name.
    static std::string VERSION;     ///< Defines the version XML element name.
    static std::string POINT;       ///< Defines the version XML element name.
};

} // namespace io

} // namespace impl

} // namespace fwRuntime
