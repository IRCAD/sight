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

#include "activity/config.hpp"

#include <core/base_object.hpp>
#include <core/mt/types.hpp>

#include <boost/property_tree/ptree.hpp>

#include <map>
#include <string>

namespace sight
{

namespace data
{

class activity;
class vector;

} // namespace data

namespace core::runtime
{

class extension;

} // namespace core::runtime

namespace activity::extension
{

using config_t = boost::property_tree::ptree;

struct ACTIVITY_CLASS_API activity_config_param
{
    activity_config_param()
    = default;

    ACTIVITY_API activity_config_param(const config_t& _config);

    //------------------------------------------------------------------------------

    [[nodiscard]] bool is_object_path() const
    {
        return (by.substr(0, 1) == "@") || (by.substr(0, 1) == "!");
    }

    std::string replace;
    std::string by;
};

using activity_config_params_type = std::vector<activity_config_param>;

struct ACTIVITY_CLASS_API activity_config
{
    activity_config()
    = default;

    ACTIVITY_API activity_config(const config_t& _config);

    std::string id;
    activity_config_params_type parameters;
};

struct ACTIVITY_CLASS_API activity_requirement_key
{
    activity_requirement_key()
    = default;

    ACTIVITY_API activity_requirement_key(const config_t& _config);

    std::string key;
};

struct ACTIVITY_CLASS_API activity_requirement
{
    ACTIVITY_API activity_requirement() = default;
    ACTIVITY_API activity_requirement(const config_t& _config);

    using key_t = std::vector<activity_requirement_key>;

    /// parameter name
    std::string name;

    /// parameter type (ie. data::image_series)
    std::string type;

    /// data container if maxOccurs > 1 ("vector" or "composite", default: "composite")
    std::string container;

    /// parameter description
    std::string description;

    /// Implementation of data validator
    std::string validator;

    /// minimum number of data required
    unsigned int min_occurs {};

    /// maximum number of data required
    unsigned int max_occurs {};

    /// True if the data must be created if it is not present (only if minOccurs = 0 and maxOccurs = 1)
    bool create {};

    /// True if data must be reset before activity launch (only if the data has been created by the activity)
    bool reset {};

    /// Parameter key if the container == "composite"
    key_t keys;

    /// Child configuration, used for instance when invoking object parsers
    config_t object_config;
};

/**
 * @brief Holds Activity configuration.
 *
 * Activity parameters are (in this order) :
 * - \b id : activity id
 * - \b title : activity title (displayed in tab if tabinfo isn't specified)
 * - \b tabinfo : activity title (displayed in tab)
 * - \b desc : activity description
 * - \b icon : path to the icon activity
 * - \b requirements : required elements to launch specified activity (must be present in vector selection)
 *   - \b requirement : a required element
 *     - \b name : element key in Activity composite
 *     - \b type : object type
 *     - \b minOccurs (optional, default value = 1) : minimal number of object (with specified type) in vector
 *     - \b maxOccurs (optional, default value = 1) : maximal number of object (with specified type) in vector
 *       - \b key : if maxOccurs > 1, then you must defined keys for each objects
 *     - \b container (optional, default value = composite) : container type (vector or composite) to store required
 *       parameters
 *     - \b create (optional) : true if the data must be created if it is not present (only available if minOccurs = 0
 *       and maxOccurs = 1)
 *     - \b reset (optional) : true if the data must be reset if the activity is reloaded (only available if
 *                             create = true or minOccurs = 0)
 *       and maxOccurs = 1)
 *     - \b desc (optional) : description of the requirement
 *     - \b validator (optional) : validate the current data
 * - \b builder (optional): implementation of builder associate to the activity, the builder creates Activity.
 *   - default builder is activity::builder::Activity
 * - \b validator (optional): check if specified activity can be launched with selected objects
 * - \b validators (optional) : defines validators implementations instantiated to validate activity launch
 *   -  \b validator : implementation name for a validator
 * - \b appConfig : defined config launched by this activity
 *   - id : config id
 *   - \b parameters : parameters required by the config
 *     - \b parameter : defined an config parameter
 *       - replace : parameter name to replace in config
 *       - by : value to use for replacement (can be a string or object path)
 *
 * Example of activity configuration:
 * @code{.xml}
    <extension implements="sight::activity::extension::Activity">
        <id>3DVisualization</id>
        <title>3D Visu</title>
        <tabinfo>3D MPR - !values.modelSeries.patient.name</tabinfo>
        <desc>Activity description ...</desc>
        <icon>sight::module::ui::icons/icon-3D.png</icon>
        <requirements>
            <requirement name="param1" type="data::image" /> <!-- defaults : minOccurs = 1, maxOccurs = 1-->
            <requirement name="param2" type="data::mesh" maxOccurs="3" >
                <key>Item1</key>
                <key>Item2</key>
                <key>Item3</key>
            </requirement>
            <requirement name="param3" type="data::mesh" maxOccurs="*" container="vector" />
            <requirement name="imageSeries" type="data::image_series" minOccurs="0" maxOccurs="2" />
            <requirement name="modelSeries" type="data::model_series" minOccurs="1" maxOccurs="1">
                 <desc>Description of the required data....</desc>
                 <validator>activity::validator::image_properties</validator>
            </requirement>
            <requirement name="transformationMatrix" type="data::matrix4" minOccurs="0" maxOccurs="1"
 * create="true" />
            <!--# ...-->
        </requirements>
        <builder>activity::builder::Activity</builder>
        <validators>
            <validator>activity::validator::related_study</validator>
        </validators>
        <appConfig id="3DVisualization">
            <parameters>
                <parameter replace="orientation" by="frontal" />
                <!--# ...-->
            </parameters>
        </appConfig>
    </extension>
 * @endcode
 */
struct ACTIVITY_CLASS_API activity_info
{
    using requirements_t             = std::vector<activity_requirement>;
    using min_max_t                  = std::pair<unsigned int, unsigned int>;
    using requirements_min_max_count = std::map<std::string, min_max_t>;
    using data_count_t               = std::map<std::string, unsigned int>;

    ACTIVITY_API activity_info() = default;
    ACTIVITY_API activity_info(const SPTR(core::runtime::extension)& _ext);

    [[nodiscard]] ACTIVITY_API bool usable_with(data_count_t _data_count) const;

    std::string id;
    std::string title;
    std::string description;
    std::string icon;
    std::string tab_info;
    requirements_t requirements;
    std::string builder_impl;
    std::string bundle_id; ///< Identifier of the module containing the activity

    /// Validator implementations
    std::vector<std::string> validators_impl;

    activity_config app_config;

    protected:

        requirements_min_max_count m_requirement_count;
};

/**
 * @brief This class allows to register all the configuration parameters which has the point extension
 *        "sight::extension::Activity".
 *
 * @see activity::extension::activity_info
 */
class ACTIVITY_CLASS_API activity : public core::base_object
{
public:

    using infos_t = std::vector<activity_info>;

    SIGHT_DECLARE_CLASS(activity, core::base_object);

    /// Return the default global instance of Activity
    ACTIVITY_API static activity::sptr get_default();

    /// Destructor
    ACTIVITY_API activity()           = default;
    ACTIVITY_API ~activity() override = default;

    /**
     * @brief Parse module information to retrieve config parameters declaration
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     **/
    ACTIVITY_API void parse_plugin_infos();
    ///@}

    /**
     * @brief Get the parameters associated to extension id.
     * @note This method is thread safe.
     **/
    ACTIVITY_API activity_info get_info(const std::string& _extension_id) const;

    /**
     * @brief Tests if we have information about operator
     * @note This method is thread safe.
     */
    ACTIVITY_API bool has_info(const std::string& _extension_id) const;

    /**
     * @brief Get the number of vector objects in the same type.
     */
    static ACTIVITY_API activity_info::data_count_t get_data_count(const CSPTR(data::vector)& _data);

    /**
     * @brief Get all infos
     * @note This method is thread safe.
     */
    ACTIVITY_API infos_t get_infos() const;

    /**
     * @brief Get available activities for given data.
     * @note This method is thread safe.
     */
    ACTIVITY_API infos_t get_infos(const CSPTR(data::vector)& _data) const;

    /**
     * @brief Get all keys
     * @note This method is thread safe.
     */
    ACTIVITY_API std::vector<std::string> get_keys() const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    ACTIVITY_API void clear_registry();

    ACTIVITY_API std::tuple<activity_info, std::map<std::string, std::string> > get_info_and_replacement_map(
        const data::activity& _activity,
        const activity_config_params_type& _parameters = activity_config_params_type()
    ) const;

    static ACTIVITY_API std::map<std::string, std::string> get_replacement_map(
        const data::activity& _activity,
        const activity_info& _info,
        const activity_config_params_type& _parameters = activity_config_params_type()
    );

protected:

    using registry = std::map<std::string, activity_info>;

    /// Container of parameter information
    registry m_reg;

    /// Used to protect the registry access.
    mutable core::mt::read_write_mutex m_registry_mutex;
};

} // namespace activity::extension

} // namespace sight
