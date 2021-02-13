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

#include "activity/config.hpp"
#include "activity/registry/Activity.hpp"

#include <data/ActivitySeries.hpp>
#include <data/mt/weak_ptr.hpp>
#include <data/Series.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::activity
{

/**
 * @brief Base class for services that launches Activity
 */
class ACTIVITY_CLASS_API IActivityLauncher
{
public:

    typedef ::boost::property_tree::ptree ConfigurationType;
    typedef activity::registry::ActivityAppConfigParam ParameterType;
    typedef activity::registry::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;
    typedef std::map< std::string, std::string> ReplaceMapType;
    typedef std::map< std::string, data::mt::weak_ptr< data::Object > > InOutMapType;

    /// Constructor. Do nothing.
    ACTIVITY_API IActivityLauncher();

    /// Destructor. Do nothing.
    ACTIVITY_API virtual ~IActivityLauncher();

protected:

    /// Parse the configuration
    ACTIVITY_API virtual void parseConfiguration(const ConfigurationType& config,
                                                 const InOutMapType& inouts = InOutMapType());

    /// Create the activity series given in 'mainActivity' configuration
    ACTIVITY_API virtual data::ActivitySeries::sptr createMainActivity() const;

    /**
     * @brief Check if the activity is valid by calling the activity validator.
     * @return Return true if the given activity is valid
     */
    ACTIVITY_API std::pair<bool, std::string> validateActivity(
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
    ACTIVITY_API void translateParameters(const data::Object::csptr& sourceObj,
                                          const ParametersType& parameters,
                                          ReplaceMapType& replaceMap );

    /**
     * @brief Create the replace map from the parameters.
     *
     * @param[in] parameters list of parameters to translate
     * @param[out] replaceMap map containing the translated parameter
     */
    ACTIVITY_API void translateParameters( const ParametersType& parameters, ReplaceMapType& replaceMap );

    std::string m_mainActivityId; ///< configuration id of the main activity

    ParametersType m_parameters; ///< parameters given in configuration
};

} // namespace sight::activity
