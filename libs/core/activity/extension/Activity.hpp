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

#pragma once

#include "activity/config.hpp"

#include <core/BaseObject.hpp>
#include <core/mt/types.hpp>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/EConfigurationElement.hpp>

#include <boost/property_tree/ptree.hpp>

#include <map>
#include <string>

namespace sight
{

namespace data
{

class ActivitySeries;
class Vector;

}

namespace core::runtime
{

struct Extension;

}

namespace activity
{

namespace extension
{

typedef boost::property_tree::ptree ConfigType;

struct ACTIVITY_CLASS_API ActivityAppConfigParam
{
    ActivityAppConfigParam()
    {
    }

    ACTIVITY_API ActivityAppConfigParam(const ConfigType& config);

    //------------------------------------------------------------------------------

    bool isObjectPath() const
    {
        return (by.substr(0, 1) == "@") || (by.substr(0, 1) == "!");
    }

    std::string replace;
    std::string by;
};

using ActivityAppConfigParamsType = std::vector<ActivityAppConfigParam>;

struct ACTIVITY_CLASS_API ActivityAppConfig
{
    ActivityAppConfig()
    {
    }

    ACTIVITY_API ActivityAppConfig(const ConfigType& config);

    std::string id;
    ActivityAppConfigParamsType parameters;
};

struct ACTIVITY_CLASS_API ActivityRequirementKey
{
    ActivityRequirementKey()
    {
    }

    ACTIVITY_API ActivityRequirementKey(const ConfigType& config);

    std::string key;
};

struct ACTIVITY_CLASS_API ActivityRequirement
{
    ActivityRequirement()
    {
    }

    ACTIVITY_API ActivityRequirement(const ConfigType& config);

    typedef std::vector<ActivityRequirementKey> KeyType;

    std::string name;        /// parameter name
    std::string type;        /// parameter type (ie. data::ImageSeries)
    std::string container;   /// data container if maxOccurs > 1 ("vector" or "composite", default: "composite")
    std::string description; /// parameter description
    std::string validator;   /// Implementation of data validator
    unsigned int minOccurs;  /// minimum number of data required
    unsigned int maxOccurs;  /// maximum number of data required
    bool create;             /// True if the data must be created if it is not present (only if minOccurs = 0 and
                             /// maxOccurs = 1)
    KeyType keys;            /// Parameter key if the container == "composite"
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
 *     - \b name : element key in ActivitySeries composite
 *     - \b type : object type
 *     - \b minOccurs (optional, default value = 1) : minimal number of object (with specified type) in vector
 *     - \b maxOccurs (optional, default value = 1) : maximal number of object (with specified type) in vector
 *       - \b key : if maxOccurs > 1, then you must defined keys for each objects
 *     - \b container (optional, default value = composite) : container type (vector or composite) to store required
 *       parameters
 *     - \b create (optional) : true if the data must be created if it is not present (only available if minOccurs = 0
 *       and maxOccurs = 1)
 *     - \b desc (optional) : description of the requirement
 *     - \b validator (optional) : validate the current data
 * - \b builder (optional): implementation of builder associate to the activity, the builder creates ActivitySeries.
 *   - default builder is activity::builder::ActivitySeries
 * - \b validator (optional): check if specified activity can be launched with selected objects
 * - \b validators (optional) : defines validators implementations instantiated to validate activity launch
 *   -  \b validator : implementation name for a validator
 * - \b appConfig : defined AppConfig launched by this activity
 *   - id : AppConfig id
 *   - \b parameters : parameters required by the AppConfig
 *     - \b parameter : defined an AppConfig parameter
 *       - replace : parameter name to replace in AppConfig
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
            <requirement name="param1" type="data::Image" /> <!-- defaults : minOccurs = 1, maxOccurs = 1-->
            <requirement name="param2" type="data::Mesh" maxOccurs="3" >
                <key>Item1</key>
                <key>Item2</key>
                <key>Item3</key>
            </requirement>
            <requirement name="param3" type="data::Mesh" maxOccurs="*" container="vector" />
            <requirement name="imageSeries" type="data::ImageSeries" minOccurs="0" maxOccurs="2" />
            <requirement name="modelSeries" type="data::ModelSeries" minOccurs="1" maxOccurs="1">
                 <desc>Description of the required data....</desc>
                 <validator>activity::validator::ImageProperties</validator>
            </requirement>
            <requirement name="transformationMatrix" type="data::Matrix4" minOccurs="0" maxOccurs="1"
 * create="true" />
            <!--# ...-->
        </requirements>
        <builder>activity::builder::ActivitySeries</builder>
        <validators>
            <validator>activity::validator::RelatedStudy</validator>
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
struct ACTIVITY_CLASS_API ActivityInfo
{
    typedef std::vector<ActivityRequirement> RequirementsType;
    typedef std::pair<unsigned int, unsigned int> MinMaxType;
    typedef std::map<std::string, MinMaxType> RequirementsMinMaxCount;
    typedef std::map<std::string, unsigned int> DataCountType;

    ActivityInfo()
    {
    }

    ACTIVITY_API ActivityInfo(const SPTR(core::runtime::Extension)& ext);

    ACTIVITY_API bool usableWith(DataCountType dataCount) const;

    std::string id;
    std::string title;
    std::string description;
    std::string icon;
    std::string tabInfo;
    RequirementsType requirements;
    std::string builderImpl;
    std::string bundleId; ///< Identifier of the module containing the activity

    /// Validator implementations
    std::vector<std::string> validatorsImpl;

    ActivityAppConfig appConfig;

    protected:

        RequirementsMinMaxCount m_requirementCount;
};

/**
 * @brief This class allows to register all the configuration parameters which has the point extension
 *        "::activityReg::extension::Activity".
 *
 * @see activity::extension::ActivityInfo
 */
class ACTIVITY_CLASS_API Activity : public core::BaseObject
{
public:

    typedef std::vector<ActivityInfo> ActivitiesType;

    SIGHT_DECLARE_CLASS(Activity, core::BaseObject, new Activity);

    /// Return the default global instance of Activity
    ACTIVITY_API static Activity::sptr getDefault();

    /// Destructor
    ACTIVITY_API virtual ~Activity();

    /**
     * @brief Parse module information to retrieve config parameters declaration
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     **/
    ACTIVITY_API void parseBundleInformation();
    ///@}

    /**
     * @brief Get the parameters associated to extension id.
     * @note This method is thread safe.
     **/
    ACTIVITY_API const ActivityInfo getInfo(const std::string& extensionId) const;

    /**
     * @brief Tests if we have information about operator
     * @note This method is thread safe.
     */
    ACTIVITY_API bool hasInfo(const std::string& extensionId) const;

    /**
     * @brief Get the number of vector objects in the same type.
     */
    ACTIVITY_API ActivityInfo::DataCountType getDataCount(const CSPTR(data::Vector)& data) const;

    /**
     * @brief Get all infos
     * @note This method is thread safe.
     */
    ACTIVITY_API ActivitiesType getInfos() const;

    /**
     * @brief Get available activities for given data.
     * @note This method is thread safe.
     */
    ACTIVITY_API ActivitiesType getInfos(const CSPTR(data::Vector)& data) const;

    /**
     * @brief Get all keys
     * @note This method is thread safe.
     */
    ACTIVITY_API std::vector<std::string> getKeys() const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    ACTIVITY_API void clearRegistry();

    ACTIVITY_API std::tuple<ActivityInfo, std::map<std::string, std::string> > getInfoAndReplacementMap(
        const data::ActivitySeries& activitySeries,
        const ActivityAppConfigParamsType& parameters = ActivityAppConfigParamsType()
    ) const;

    ACTIVITY_API std::map<std::string, std::string> getReplacementMap(
        const data::ActivitySeries& activitySeries,
        const ActivityInfo& info,
        const ActivityAppConfigParamsType& parameters = ActivityAppConfigParamsType()
    ) const;

protected:

    typedef std::map<std::string, ActivityInfo> Registry;

    /// Container of parameter information
    Registry m_reg;

    /// Constructor
    ACTIVITY_API Activity();

    /// Used to protect the registry access.
    mutable core::mt::ReadWriteMutex m_registryMutex;

    /// The global instance of the app config parameters.
    static Activity::sptr s_activity;
};

} // namespace extension

} // namespace activity

} // namespace sight
