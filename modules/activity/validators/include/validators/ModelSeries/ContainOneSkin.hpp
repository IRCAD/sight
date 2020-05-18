/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "validators/config.hpp"

#include <fwActivities/IObjectValidator.hpp>

namespace validators
{
namespace ModelSeries
{
/**
 * @brief Defines a validator which checks that the ModelSeries contains one and only one Skin.
 * @note If the given Obect is a container (Vector or Composite), it validates that each sub-object is a ModelSeries
 *       with one and only one Skin.
 */
class VALIDATORS_CLASS_API ContainOneSkin : public ::fwActivities::IObjectValidator
{
public:
    fwCoreClassMacro(ContainOneSkin, ::fwActivities::IObjectValidator,
                     ::fwActivities::validator::factory::New< ContainOneSkin >);

    /// Constructor. Do nothing.
    VALIDATORS_API ContainOneSkin(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    VALIDATORS_API virtual ~ContainOneSkin();

    /**
     * @brief Checks that the ModelSeries contains one only one Skin.
     * @note Given object should be a single ModelSeries or a Vector or a Composite of ModelSeries.
     * @see ::fwActivities::IValidator::validate
     */
    VALIDATORS_API virtual IValidator::ValidationType validate(const CSPTR(::fwData::Object)& currentObject ) const
    override;
};

} // namespace ModelSeries
} // namespace validators
