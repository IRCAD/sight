/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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
#include "fwActivities/registry/Activities.hpp"

#include <fwMedData/ActivitySeries.hpp>
#include <fwMedData/Series.hpp>

#include <boost/property_tree/ptree.hpp>

namespace fwActivities
{

/**
 * @brief Base class for services that launches Activities
 */
class FWACTIVITIES_CLASS_API IActivityLauncher
{
public:

    typedef ::boost::property_tree::ptree ConfigurationType;
    typedef ::fwActivities::registry::ActivityAppConfigParam ParameterType;
    typedef ::fwActivities::registry::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;
    typedef std::map< std::string, std::string> ReplaceMapType;
    typedef std::map< std::string, ::fwData::Object::wptr> InOutMapType;

    /// Constructor. Do nothing.
    FWACTIVITIES_API IActivityLauncher();

    /// Destructor. Do nothing.
    FWACTIVITIES_API virtual ~IActivityLauncher();

protected:

    /// Parse the configuration
    FWACTIVITIES_API virtual void parseConfiguration(const ConfigurationType& config,
                                                     const InOutMapType& inouts = InOutMapType());

    /// Create the activity series given in 'mainActivity' configuration
    FWACTIVITIES_API virtual ::fwMedData::ActivitySeries::sptr createMainActivity() const;

    /**
     * @brief Check if the activity is valid by calling the activity validator.
     * @return Return true if the given activity is valid
     */
    FWACTIVITIES_API std::pair<bool, std::string> validateActivity(
        const ::fwMedData::ActivitySeries::csptr& activitySeries) const;

    /**
     * @brief Translate parameters from source object.
     *
     * Replace camp path (ex.@values.myParam.image) by the uid of the corresponding object.
     *
     * @param[in] sourceObj source object used to find sub-object form camp path
     * @param[in] parameters list of parameters to translate
     * @param[out] replaceMap map containing the translated parameter
     */
    FWACTIVITIES_API void translateParameters(const ::fwData::Object::csptr& sourceObj,
                                              const ParametersType& parameters,
                                              ReplaceMapType& replaceMap );

    /**
     * @brief Create the replace map from the parameters.
     *
     * @param[in] parameters list of parameters to translate
     * @param[out] replaceMap map containing the translated parameter
     */
    FWACTIVITIES_API void translateParameters( const ParametersType& parameters, ReplaceMapType& replaceMap );

    std::string m_mainActivityId; ///< configuration id of the main activity

    ParametersType m_parameters; ///< parameters given in configuration
};

} // namespace fwActivities
