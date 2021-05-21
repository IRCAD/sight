/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/config.hpp"
#include "core/runtime/ConfigurationElement.hpp"
#include "core/runtime/detail/io/Validator.hpp"
#include "core/runtime/operations.hpp"
#include "core/runtime/Runtime.hpp"

#include <boost/property_tree/ptree.hpp>

namespace sight::core::runtime
{

/**
 * @brief   Update pNode content according to _cfgElement
 * @note    Used to validate a configuration element vs a xsd schema.
 * @note    Values are not managed yet (cases where curChild->type == XML_TEXT_NODE ).
 * @note    To be integrated to configuration element.
 *

 */
CORE_API void ConfigurationElement2XML(core::runtime::ConfigurationElement::sptr _cfgElement, xmlNodePtr pNode);

/**
 * @brief       Returns the configuration element embedding the configuration to be considered for initializing an
 * object or a service
 * @param[in]   _config : configuration element containing an attribute "config" specifying the name of the contribution
 * to consider
 * @param[in]   _extension_pt : extension point to which the configuration should contribute with the name indicated by
 * the previous "config" attribute value
 *
 */
CORE_API ConfigurationElement::sptr getCfgAsAnExtension(
    ConfigurationElement::sptr _config,
    std::string _extension_pt
);

/**
 * @brief   Returns configuration elements extending the _extension_pt extension point
 *

 */
CORE_API std::vector<ConfigurationElement::sptr> getAllCfgForPoint(std::string _extension_pt);

/**
 * @brief   Returns extensions extending the _extension_pt extension point
 *

 */
CORE_API std::vector<std::shared_ptr<core::runtime::Extension> > getAllExtensionsForPoint(
    std::string extension_pt
);

/**
 * @brief       Returns contribution ids and configuration elements related to a given extension point
 * @param[in]   _extension_pt : identifier of the extension point to which configuration elements contribute
 * @return      Map associating contribution ids and associated configuration elements contributing to the extension
 * point _extension_pt
 * @note        Configuration element identifiers are value of attributes "id".
 *
 */
CORE_API std::map<std::string, ConfigurationElement::sptr> getAllIdAndConfigurationElementsForPoint(
    std::string _extension_pt
);
/**
 * @brief       Returns contribution ids to a given extension point
 * @param[in]   _extension_pt : identifier of the extension point to which configuration elements contribute
 * @return      Map associating contribution ids and associated configuration elements contributing to the extension
 * point _extension_pt
 * @note        Configuration element identifiers are value of attributes "id".
 *
 */
CORE_API std::vector<std::string> getAllIdsForPoint(std::string _extension_pt);

/**
 * @brief   Get information for the point
 * @return  a string with the information of the extension point
 */
CORE_API std::string getInfoForPoint(std::string _extension_pt);

/**
 * @brief   Returns the value of a path in a boost::property::tree.
 *  This handles three different cases that a single boost function can't discriminate.
 *  1. If the path exists, this returns the value
 *  2. If the path exists but the value type is not compatible, this throws a core::Exception
 *  3. If the path does not exist, this returns the default value
 * @return  value if the path is set, default value otherwise
 * @throw   sight::core::Exception if the value type is not compatible
 */
template<typename T>
T get_ptree_value(const boost::property_tree::ptree& tree, const std::string& path, T defaultValue)
{
    boost::property_tree::ptree element;
    try
    {
        element = tree.get_child(path);
    }
    catch(const boost::property_tree::ptree_bad_path& e)
    {
        // 3.
        return defaultValue;
    }

    if(boost::optional<T> value = element.get_value_optional<T>())
    {
        // 1.
        return *value;
    }

    // 2.
    SIGHT_THROW_EXCEPTION(core::Exception("Wrong value set in path: " + path));
    return defaultValue;
}

}
