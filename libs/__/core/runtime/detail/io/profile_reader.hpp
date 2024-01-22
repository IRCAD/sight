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

#include "core/config.hpp"

#include <libxml/tree.h>

#include <filesystem>
#include <string>

namespace sight::core::runtime::detail
{

namespace profile
{

class activater;
class profile;

} // namespace profile

namespace io
{

/**
 * @brief   Reads plugin set profiles from xml files.
 */
class profile_reader
{
public:

    /**
     * @brief       Creates a profile from an xml file located at the given path.
     *
     * @param[in]   _path    a path to an xml profile file
     */
    static std::shared_ptr<core::runtime::detail::profile::profile> create_profile(
        const std::filesystem::path& _path
    );

    /**
     * @brief       Processes the given xml node as a profile.
     *
     * @param[in]   _node    a pointer to the xml node to process
     *
     * @return      a shared pointer to the created profile
     */
    static std::shared_ptr<core::runtime::detail::profile::profile> process_profile(xmlNodePtr _node);
    static std::string process_starter(xmlNodePtr _node);

    /**
     * @brief       Processes the given xml node as an activater.
     *
     * @param[in]   _node    a pointer to the xml node to process
     *
     * @return      a shared pointer to the created activater
     */
    static std::shared_ptr<core::runtime::detail::profile::activater> process_activater(xmlNodePtr _node);

    /**
     * @brief       Processes the given xml node as an activater parameter and
     *              stores it in the given activater.
     *
     * @param[in]   _node        a pointer to the xml node to process
     * @param[out]  _activater   a shared pointer to an activater
     */
    static void process_activater_param(
        xmlNodePtr _node,
        std::shared_ptr<core::runtime::detail::profile::activater> _activater
    );

    /**
     * @brief       Processes the given xml node as an activater disable extension point and
     *              stores it in the given activater.
     *
     * @param[in]   _node        a pointer to the xml node to process
     * @param[out]  _activater   a shared pointer to an activater
     */
    static void process_activater_disable_extension_point(
        xmlNodePtr _node,
        std::shared_ptr<core::runtime::detail::profile::activater> _activater
    );

    /**
     * @brief       Processes the given xml node as an activater disable extension and
     *              stores it in the given activater.
     *
     * @param[in]   _node        a pointer to the xml node to process
     * @param[out]  _activater   a shared pointer to an activater
     */
    static void process_activater_disable_extension(
        xmlNodePtr _node,
        std::shared_ptr<core::runtime::detail::profile::activater> _activater
    );

private:

    static std::string s_id;                    ///< defines the name of the id xml element or attribute name
    static std::string s_name;                  ///< defines the name of the xml profile
    static std::string s_value;                 ///< defines the name of the value xml element or attribute name
    static std::string s_version;               ///< defines the name of the version xml element or attribute name
    static std::string s_check_single_instance; ///< defines the name of the check-single-instance xml element or
    // attribute name
    static std::string s_activate;   ///< defines the name of the activate xml element or attribute name
    static std::string s_start;      ///< defines the name of the start xml element or attribute name
    static std::string s_param;      ///< defines the name of the parameter xml element or attribute name
    static std::string s_dis_ext_pt; ///< defines the name of the disable extension point xml element or attribute
    // name
    static std::string s_dis_ext; ///< defines the name of the disable extension xml element or attribute name
};

} // namespace io

} // namespace sight::core::runtime::detail
