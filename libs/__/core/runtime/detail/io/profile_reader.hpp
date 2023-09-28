/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
     * @param[in]   path    a path to an xml profile file
     */
    static std::shared_ptr<core::runtime::detail::profile::profile> create_profile(
        const std::filesystem::path& path
    );

    /**
     * @brief       Processes the given xml node as a profile.
     *
     * @param[in]   node    a pointer to the xml node to process
     *
     * @return      a shared pointer to the created profile
     */
    static std::shared_ptr<core::runtime::detail::profile::profile> process_profile(xmlNodePtr node);
    static std::string process_starter(xmlNodePtr node);

    /**
     * @brief       Processes the given xml node as an activater.
     *
     * @param[in]   node    a pointer to the xml node to process
     *
     * @return      a shared pointer to the created activater
     */
    static std::shared_ptr<core::runtime::detail::profile::activater> process_activater(xmlNodePtr node);

    /**
     * @brief       Processes the given xml node as an activater parameter and
     *              stores it in the given activater.
     *
     * @param[in]   node        a pointer to the xml node to process
     * @param[out]  activater   a shared pointer to an activater
     */
    static void process_activater_param(
        xmlNodePtr node,
        std::shared_ptr<core::runtime::detail::profile::activater> activater
    );

    /**
     * @brief       Processes the given xml node as an activater disable extension point and
     *              stores it in the given activater.
     *
     * @param[in]   node        a pointer to the xml node to process
     * @param[out]  activater   a shared pointer to an activater
     */
    static void process_activater_disable_extension_point(
        xmlNodePtr node,
        std::shared_ptr<core::runtime::detail::profile::activater> activater
    );

    /**
     * @brief       Processes the given xml node as an activater disable extension and
     *              stores it in the given activater.
     *
     * @param[in]   node        a pointer to the xml node to process
     * @param[out]  activater   a shared pointer to an activater
     */
    static void process_activater_disable_extension(
        xmlNodePtr node,
        std::shared_ptr<core::runtime::detail::profile::activater> activater
    );

private:

    static std::string ID;                    ///< defines the name of the id xml element or attribute name
    static std::string NAME;                  ///< defines the name of the xml profile
    static std::string VALUE;                 ///< defines the name of the value xml element or attribute name
    static std::string VERSION;               ///< defines the name of the version xml element or attribute name
    static std::string CHECK_SINGLE_INSTANCE; ///< defines the name of the check-single-instance xml element or
    // attribute name
    static std::string ACTIVATE;   ///< defines the name of the activate xml element or attribute name
    static std::string START;      ///< defines the name of the start xml element or attribute name
    static std::string PARAM;      ///< defines the name of the parameter xml element or attribute name
    static std::string DIS_EXT_PT; ///< defines the name of the disable extension point xml element or attribute
    // name
    static std::string DIS_EXT; ///< defines the name of the disable extension xml element or attribute name
};

} // namespace io

} // namespace sight::core::runtime::detail
