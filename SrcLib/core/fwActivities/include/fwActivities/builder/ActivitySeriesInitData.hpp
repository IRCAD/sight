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

#include "fwActivities/builder/ActivitySeries.hpp"
#include "fwActivities/config.hpp"

#include <fwCore/base.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwActivities
{
namespace builder
{

/**
 * @brief Defines an activity builder which instanciates a new requirement if minOccurs and maxOccurs are set to 0.
 */
class FWACTIVITIES_CLASS_API ActivitySeriesInitData : public ::fwActivities::builder::ActivitySeries
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ActivitySeriesInitData)(::fwActivities::IBuilder),
                                            ::fwActivities::builder::factory::New< ActivitySeriesInitData > )

    typedef std::string RegistryKeyType;

    /// Constructor. Do nothing.
    FWACTIVITIES_API ActivitySeriesInitData(::fwActivities::IBuilder::Key key);

    /// Destructor. Do nothing.
    FWACTIVITIES_API virtual ~ActivitySeriesInitData();

    /**
     * @brief Instanciates a new object where requirement maxOccurs and minOccurs are both set to 0.
     * @see ::fwActivities::builder::ActivitySeries::buildData
     */
    FWACTIVITIES_API virtual ::fwMedData::ActivitySeries::sptr buildData(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        const CSPTR(::fwData::Vector)& currentSelection ) const override;
};

} // namespace builder
} // namespace fwActivities
