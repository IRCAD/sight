/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "activities/config.hpp"
#include "activities/registry/Activities.hpp"

#include <data/ActivitySeries.hpp>
#include <data/mt/weak_ptr.hpp>
#include <data/Series.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::activities
{

/**
 * @brief Base class for services that launches Activities
 */
class ACTIVITIES_CLASS_API IActivityLauncher
{
public:

    typedef ::boost::property_tree::ptree ConfigurationType;
    typedef activities::registry::ActivityAppConfigParam ParameterType;
    typedef activities::registry::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;
    typedef std::map< std::string, std::string> ReplaceMapType;
    typedef std::map< std::string, data::mt::weak_ptr< data::Object > > InOutMapType;

    /// Constructor. Do nothing.
    ACTIVITIES_API IActivityLauncher();

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~IActivityLauncher();

protected:

    /// Parse the configuration
    ACTIVITIES_API virtual void parseConfiguration(const ConfigurationType& config,
                                                   const InOutMapType& inouts = InOutMapType());

    /// Create the activity series given in 'mainActivity' configuration
    ACTIVITIES_API virtual data::ActivitySeries::sptr createMainActivity() const;

    /**
     * @brief Check if the activity is valid by calling the activity validator.
     * @return Return true if the given activity is valid
     */
    ACTIVITIES_API std::pair<bool, std::string> validateActivity(
        const data::ActivitySeries::csptr& activitySeries) const;

    /**
     * @brief Translate parameters from source object.
     *
     * Replace camp path (ex.@values.myParam.image) by the uid of the corresponding object.
     *
     * @param[in] sourceObj source object used to find sub-object form camp path
     * @param[in] parameters list of parameters to translate
     * @param[out] replaceMap map containing the translated parameter
     */
    ACTIVITIES_API void translateParameters(const data::Object::csptr& sourceObj,
                                            const ParametersType& parameters,
                                            ReplaceMapType& replaceMap );

    /**
     * @brief Create the replace map from the parameters.
     *
     * @param[in] parameters list of parameters to translate
     * @param[out] replaceMap map containing the translated parameter
     */
    ACTIVITIES_API void translateParameters( const ParametersType& parameters, ReplaceMapType& replaceMap );

    std::string m_mainActivityId; ///< configuration id of the main activity

    ParametersType m_parameters; ///< parameters given in configuration
};

} // namespace sight::activities
