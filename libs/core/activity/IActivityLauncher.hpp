/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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
#include "activity/extension/Activity.hpp"

#include <data/Activity.hpp>

#include <boost/property_tree/ptree.hpp>

#include <optional>

namespace sight::activity
{

/**
 * @brief Base class for services that launches Activity instances.
 */
class ACTIVITY_CLASS_API IActivityLauncher
{
public:

    typedef boost::property_tree::ptree ConfigurationType;
    typedef activity::extension::ActivityAppConfigParam ParameterType;
    typedef activity::extension::ActivityAppConfigParamsType ParametersType;
    typedef std::map<std::string, std::string> ReplaceMapType;
    typedef std::vector<std::string> InOutMapType;

    /// Constructor. Do nothing.
    ACTIVITY_API IActivityLauncher() = default;

    /// Destructor. Do nothing.
    ACTIVITY_API virtual ~IActivityLauncher() = default;

protected:

    /// Parses the configuration
    ACTIVITY_API virtual void parseConfiguration(
        const ConfigurationType& config,
        const InOutMapType& inouts = InOutMapType()
    );

    /// Create the activity given in 'mainActivity' configuration
    [[nodiscard]] ACTIVITY_API virtual data::Activity::sptr createMainActivity() const;

    /**
     * @brief Check if the activity is valid by calling the activity validator.
     * @return Return true if the given activity is valid
     */
    static ACTIVITY_API std::pair<bool, std::string> validateActivity(
        const data::Activity::csptr& activity
    );

    std::string m_mainActivityId; ///< configuration id of the main activity

    ParametersType m_parameters; ///< parameters given in configuration
};

} // namespace sight::activity
