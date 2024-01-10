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

#pragma once

#include "core/runtime/detail/dl/library.hpp"
#include "core/runtime/detail/extension_point.hpp"
#include "core/runtime/runtime_exception.hpp"

#include <libxml/parser.h>

#include <filesystem>
#include <string>
#include <vector>

namespace sight::core::runtime
{

class module;
class extension;

namespace detail
{

class module;
class extension_point;

namespace io
{

/**
 * @brief   Defines the module descriptor reader class.
 */
class module_descriptor_reader
{
public:

    using module_container = std::vector<std::shared_ptr<module> >;
    /// Pair of created extension point associated with extensions
    using point_extensions_pair_type = std::pair<std::shared_ptr<extension_point>,
                                                 std::vector<std::shared_ptr<extension> > >;

    /**
     * @brief       Creates all modules that are found at the given location.
     *
     * @param[in]   location    a relative or absolute path to a directory containing modules
     *
     * @return      path to the absolute location path and a vector of all created modules
     */
    static std::pair<std::filesystem::path, module_container> create_modules(const std::filesystem::path& _location);

    /**
     * @brief       Look for a descriptor at the specified location,
     *              reads it and creates a module with it.
     *
     * @param[in]   location    a path to a directory that should contain a module description
     *
     * @return      a shared pointer to the created module
     */
    static std::shared_ptr<module> create_module(const std::filesystem::path& _location);

    /**
     * @brief   Processes a configuration element XML node.
     *
     * @param   node          a pointer to the XML node that represents a configuration element
     * @param   parentConfig  the configuration of the parent
     *
     * @return  a pointer to the created configuration element
     */
    static void process_configuration(xmlNodePtr _node, core::runtime::config_t& _parent_config);

    /**
     * @brief   Processes an extension XML node.
     *
     * @param   node    a pointer to the XML node that represents an extension
     * @param   module  a pointer to the module the extension will be attached to
     *
     * @return  a pointer to the created extension
     */
    static std::shared_ptr<extension> process_extension(xmlNodePtr _node, const std::shared_ptr<module> _module);

    /**
     * Processes a node that is a point
     *
     * @param   node    a pointer to the xml document node that represents a requirement
     * @param   module  a pointer to the module the extension will be attached to
     * @return  a pair with the created extension point and extensions
     */
    static point_extensions_pair_type process_point(xmlNodePtr _node, const std::shared_ptr<module> _module);

    /**
     * @brief   Processes an extension point XML node.
     *
     * @param   node    a pointer to the XML node that represents an extension point
     * @param   module  a pointer to the module declaring the extension point
     *
     * @return  a pointer to the created extension point
     */
    static std::shared_ptr<extension_point> process_extension_point(
        xmlNodePtr _node,
        const std::shared_ptr<module> _module
    );

    /**
     * Processes a node that is a plugin element.
     *
     * @param   node        a pointer to the xml document node that represents a module
     * @param   location    a path to a directory containing the module
     *
     * @return  a pointer to the created module
     */
    static std::shared_ptr<detail::module> process_plugin(xmlNodePtr _node, const std::filesystem::path& _location);

    /**
     * Processes a node that is a requirement
     *
     * @param   node        a pointer to the xml document node that represents a requirement
     *
     * @return  a string containing the requirement's value
     */
    static std::string process_requirement(xmlNodePtr _node);
};

} // namespace io

} // namespace detail

} // namespace sight::core::runtime
