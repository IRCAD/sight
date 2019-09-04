/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwActivities/config.hpp"

#include <fwCore/BaseObject.hpp>
#include <fwCore/mt/types.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <boost/property_tree/ptree.hpp>

#include <map>
#include <string>

namespace fwData
{
class Vector;
}

namespace fwRuntime
{
struct Extension;
}

namespace fwActivities
{

namespace registry
{

typedef ::boost::property_tree::ptree ConfigType;

struct FWACTIVITIES_CLASS_API ActivityAppConfigParam
{
    ActivityAppConfigParam()
    {
    }
    FWACTIVITIES_API ActivityAppConfigParam(const ConfigType& config);

    //------------------------------------------------------------------------------

    bool isSeshat() const
    {
        return (by.substr(0, 1) == "@") || (by.substr(0, 1) == "!");
    }

    std::string replace;
    std::string by;
};

struct FWACTIVITIES_CLASS_API ActivityAppConfig
{
    typedef std::vector< ActivityAppConfigParam > ActivityAppConfigParamsType;

    ActivityAppConfig()
    {
    }
    FWACTIVITIES_API ActivityAppConfig(const ConfigType& config);

    std::string id;
    ActivityAppConfigParamsType parameters;
};

struct FWACTIVITIES_CLASS_API ActivityRequirementKey
{
    ActivityRequirementKey()
    {
    }
    FWACTIVITIES_API ActivityRequirementKey(const ConfigType& config);

    std::string key;
    std::string path;
};

struct FWACTIVITIES_CLASS_API ActivityRequirement
{
    ActivityRequirement()
    {
    }
    FWACTIVITIES_API ActivityRequirement(const ConfigType& config);

    typedef std::vector< ActivityRequirementKey > KeyType;

    std::string name; /// parameter name
    std::string type; /// parameter type (ie. ::fwMedData::ImageSeries)
    std::string container; /// data container if maxOccurs > 1 ("vector" or "composite", default: "composite")
    std::string description; /// parameter description
    std::string validator;  /// Implementation of data validator
    unsigned int minOccurs; /// minimum number of data required
    unsigned int maxOccurs; /// maximum number of data required
    bool create; /// True if the data must be created if it is not present (only if minOccurs = 0 and maxOccurs = 1)
    KeyType keys; /// Parameter key if the container == "composite"
};

/**
 * @brief Holds Activities configuration.
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
 *   - default builder is ::fwActivities::builder::ActivitySeries
 * - \b validator (optional): check if specified activity can be launched with selected objects
 * - \b validators (optional) : defines validators implementations instantiated to validate activity launch
 *   -  \b validator : implementation name for a validator
 * - \b appConfig : defined AppConfig launched by this activity
 *   - id : AppConfig id
 *   - \b parameters : parameters required by the AppConfig
 *     - \b parameter : defined an AppConfig parameter
 *       - replace : parameter name to replace in AppConfig
 *       - by : value to use for replacement (can be a string or sesh@ path)
 *
 * Example of activity configuration:
 * @code{.xml}
    <extension implements="::fwActivities::registry::Activities">
        <id>3DVisualization</id>
        <title>3D Visu</title>
        <tabinfo>3D MPR - !values.modelSeries.patient.name</tabinfo>
        <desc>Activity description ...</desc>
        <icon>media-0.1/icons/icon-3D.png</icon>
        <requirements>
            <requirement name="param1" type="::fwData::Image" /> <!-- defaults : minOccurs = 1, maxOccurs = 1-->
            <requirement name="param2" type="::fwData::Mesh" maxOccurs="3" >
                <key>Item1</key>
                <key>Item2</key>
                <key>Item3</key>
            </requirement>
            <requirement name="param3" type="::fwData::Mesh" maxOccurs="*" container="vector" />
            <requirement name="imageSeries" type="::fwMedData::ImageSeries" minOccurs="0" maxOccurs="2" />
            <requirement name="modelSeries" type="::fwMedData::ModelSeries" minOccurs="1" maxOccurs="1">
                 <desc>Description of the required data....</desc>
                 <validator>::fwActivities::validator::ImageProperties</validator>
            </requirement>
            <requirement name="transformationMatrix" type="::fwData::TransformationMatrix3D" minOccurs="0" maxOccurs="1"
 * create="true" />
            <!--# ...-->
        </requirements>
        <builder>::fwActivities::builder::ActivitySeries</builder>
        <validators>
            <validator>::fwActivities::validator::RelatedStudy</validator>
        </validators>
        <appConfig id="3DVisualization">
            <parameters>
                <parameter replace="registeredImageUid" by="@values.param1" />
                <parameter replace="orientation" by="frontal" />
                <!--# ...-->
            </parameters>
        </appConfig>
    </extension>
 * @endcode
 */
struct FWACTIVITIES_CLASS_API ActivityInfo
{
    typedef std::vector< ActivityRequirement > RequirementsType;
    typedef std::pair<unsigned int, unsigned int> MinMaxType;
    typedef std::map< std::string, MinMaxType > RequirementsMinMaxCount;
    typedef std::map< std::string, unsigned int > DataCountType;

    ActivityInfo()
    {
    }

    FWACTIVITIES_API ActivityInfo(const SPTR(::fwRuntime::Extension)& ext);

    FWACTIVITIES_API bool usableWith(DataCountType dataCount) const;

    std::string id;
    std::string title;
    std::string description;
    std::string icon;
    std::string tabInfo;
    RequirementsType requirements;
    std::string builderImpl;
    std::string bundleId; ///< Identifier of the bundle containing the activity
    std::string bundleVersion; ///< Version of the bundle containing the activity

    /// Validator implementations
    std::vector< std::string > validatorsImpl;

    ActivityAppConfig appConfig;

    protected:

        RequirementsMinMaxCount m_requirementCount;
};

/**
 * @brief This class allows to register all the configuration parameters which has the point extension
 *        "::activityReg::registry::Activities".
 *
 * @see ::fwActivities::registry::ActivityInfo
 */
class FWACTIVITIES_CLASS_API Activities : public ::fwCore::BaseObject
{

public:
    typedef std::vector< ActivityInfo > ActivitiesType;

    fwCoreClassDefinitionsWithFactoryMacro( (Activities)(::fwCore::BaseObject), new Activities );

    /// Return the default global instance of Activities
    FWACTIVITIES_API static Activities::sptr getDefault();

    /// Destructor
    FWACTIVITIES_API virtual ~Activities();

    /**
     * @brief Parse bundle information to retrieve config parameters declaration
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     * @{
     **/
    FWACTIVITIES_API void parseBundleInformation();
    FWACTIVITIES_API void parseBundleInformation(const std::vector< SPTR( ::fwRuntime::Extension ) >& extensions);
    ///@}

    /**
     * @brief Get the parameters associated to extension id.
     * @note This method is thread safe.
     **/
    FWACTIVITIES_API const ActivityInfo getInfo( const std::string& extensionId ) const;

    /**
     * @brief Tests if we have information about operator
     * @note This method is thread safe.
     */
    FWACTIVITIES_API bool hasInfo( const std::string& extensionId ) const;

    /**
     * @brief Get the number of vector objects in the same type.
     */
    FWACTIVITIES_API ActivityInfo::DataCountType getDataCount( const CSPTR(::fwData::Vector)& data ) const;

    /**
     * @brief Get all infos
     * @note This method is thread safe.
     */
    FWACTIVITIES_API ActivitiesType getInfos() const;

    /**
     * @brief Get available activities for given data.
     * @note This method is thread safe.
     */
    FWACTIVITIES_API ActivitiesType getInfos( const CSPTR(::fwData::Vector)& data ) const;

    /**
     * @brief Get all keys
     * @note This method is thread safe.
     */
    FWACTIVITIES_API std::vector< std::string > getKeys() const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    FWACTIVITIES_API void clearRegistry();

protected:

    typedef std::map< std::string, ActivityInfo > Registry;

    /// Container of parameter information
    Registry m_reg;

    /// Constructor
    FWACTIVITIES_API Activities();

    /// Used to protect the registry access.
    mutable ::fwCore::mt::ReadWriteMutex m_registryMutex;

    /// The global instance of the app config parameters.
    static Activities::sptr s_activities;

};

} // namespace registry

} // namespace fwActivities
