/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "io/patch/config.hpp"

#include <io/atoms/patch/IStructuralPatch.hpp>

namespace sight::io::patch::structural
{
namespace data
{
namespace ImageSeries
{

/// Structural patch to convert a data::ImageSeries from version '3' to version '2'.
class IO_PATCH_CLASS_API V3ToV2 : public io::atoms::patch::IStructuralPatch
{

public:

    SIGHT_DECLARE_CLASS(V3ToV2, patch::structural::data::ImageSeries::V3ToV2, new V3ToV2)

    /// Initialiaze members.
    IO_PATCH_API V3ToV2();

    /// Deletes resources.
    IO_PATCH_API ~V3ToV2();

    /**
     * @brief Copies the patch.
     * @param _cpy the patch to copy.
     */
    IO_PATCH_API V3ToV2( const V3ToV2& cpy );

    /**
     * @brief Applies patch.
     *
     * Removes attributes contrast_bolus_agent, contrast_bolus_route, contrast_bolus_volume,
     * contrast_bolus_start_time, contrast_bolus_stop_time, contrast_bolus_total_dose,
     * contrast_bolus_flow_rate, contrast_bolus_flow_duration, contrast_bolus_ingredient,
     * contrast_bolus_ingredient_concentration.
     */
    IO_PATCH_API virtual void apply(
        const sight::atoms::Object::sptr& _previous,
        const sight::atoms::Object::sptr& _current,
        io::atoms::patch::IPatch::NewVersionsType& _newVersions) override;

};

} // namespace ImageSeries
} // namespace data
} // namespace sight::io::patch::structural
