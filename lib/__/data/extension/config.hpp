/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <sight/data/config.hpp>

#include <core/object.hpp>

#include <boost/property_tree/ptree.hpp>

#include <map>
#include <shared_mutex>

namespace sight::data::extension
{

/**
 */
class SIGHT_DATA_CLASS_API data_config_info final
{
public:

    std::string data;
    std::string desc;
    boost::property_tree::ptree config;
};

/**
 * @brief This class allows to register all the data configuration which has the point extension
 *        "sight::data::extension::config".
 *
 * Registry config like :
 * @code{.xml}
    <extension implements="sight::data::extension::config">
        <id>probe_settings_config</id>
        <object>sight::data::map</object>
        <desc></desc>
        <config>

        </config>
    </extension>
   @endcode
 */
class SIGHT_DATA_CLASS_API config final
{
public:

    using sptr = std::shared_ptr<config>;

    /// Return the default global instance of config
    SIGHT_DATA_API static config::sptr get();

    /**
     * @brief Parses module information to retrieve data declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    SIGHT_DATA_API void parse_plugin_infos();

    /**
     * @brief Register a new data configuration
     * @param _config_id the identifier of the registered configuration.
     * @param _data the implementation of the data
     * @param _desc the description of the configuration
     * @param _config the registered config
     * @note This method is thread safe
     */
    SIGHT_DATA_API void add_data_config_info(
        const std::string& _config_id,
        const std::string& _data,
        const std::string& _desc,
        const boost::property_tree::ptree& _config
    );

    /**
     * @brief Returns the configuration with the given id for the data with the given implementation
     * @note This method is thread safe
     */
    SIGHT_DATA_API boost::property_tree::ptree get_data_config(
        const std::string& _config_id,
        const std::string& _data_impl = std::string()
    ) const;

    /**
     * @brief Returns the description of the given configuration name.
     * @note This method is thread safe
     */
    SIGHT_DATA_API const std::string& get_config_desc(const std::string& _config_id) const;

    /**
     * @brief Returns a vector containing the names of the available config for the data with the given
     * implementation
     * @note This method is thread safe
     */
    SIGHT_DATA_API std::vector<std::string> get_all_config_for_data(
        std::string _data_impl,
        bool _matching_only = false
    ) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    SIGHT_DATA_API void clear_registry();

private:

    /// Container of data information <configId, data config information>
    std::map<std::string, data_config_info> m_registry;

    const static std::string CONFIG_EXT_POINT;

    /// Used to protect the registry access.
    mutable std::shared_mutex m_registry_mutex;

    /// The global instance of the data config.
    static config::sptr s_data_config;
};

} // namespace sight::data::extension
