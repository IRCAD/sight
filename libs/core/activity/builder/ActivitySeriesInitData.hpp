/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "activity/builder/ActivitySeries.hpp"
#include "activity/config.hpp"

#include <core/base.hpp>
#include <core/runtime/ConfigurationElement.hpp>

namespace sight::activity
{

namespace builder
{

/**
 * @brief Defines an activity builder which instantiates a new requirement if minOccurs and maxOccurs are set to 0.
 */
class ACTIVITY_CLASS_API ActivitySeriesInitData : public activity::builder::ActivitySeries
{
public:

    SIGHT_DECLARE_CLASS(
        ActivitySeriesInitData,
        activity::IBuilder,
        activity::builder::factory::New<ActivitySeriesInitData>
    )

    typedef std::string RegistryKeyType;

    /// Constructor. Do nothing.
    ACTIVITY_API ActivitySeriesInitData(activity::IBuilder::Key key);

    /// Destructor. Do nothing.
    ACTIVITY_API virtual ~ActivitySeriesInitData();

    /**
     * @brief Instanciates a new object where requirement maxOccurs and minOccurs are both set to 0.
     * @see activity::builder::ActivitySeries::buildData
     */
    ACTIVITY_API virtual data::ActivitySeries::sptr buildData(
        const activity::extension::ActivityInfo& activityInfo,
        const CSPTR(data::Vector)& currentSelection
    ) const override;
};

} // namespace builder

} // namespace sight::activity
