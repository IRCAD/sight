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

#include "fwStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace fwStructuralPatch
{
namespace sight::data
{
namespace ImageSeries
{

/// Structural patch to convert a data::ImageSeries from version '3' to version '2'.
class FWSTRUCTURALPATCH_CLASS_API V3ToV2 : public ::fwAtomsPatch::IStructuralPatch
{

public:

    fwCoreClassMacro(V3ToV2, ::fwStructuralPatchdata::ImageSeries::V3ToV2, new V3ToV2)

    /// Initialiaze members.
    FWSTRUCTURALPATCH_API V3ToV2();

    /// Deletes resources.
    FWSTRUCTURALPATCH_API ~V3ToV2();

    /**
     * @brief Copies the patch.
     * @param _cpy the patch to copy.
     */
    FWSTRUCTURALPATCH_API V3ToV2( const V3ToV2& cpy );

    /**
     * @brief Applies patch.
     *
     * Removes attributes contrast_bolus_agent, contrast_bolus_route, contrast_bolus_volume,
     * contrast_bolus_start_time, contrast_bolus_stop_time, contrast_bolus_total_dose,
     * contrast_bolus_flow_rate, contrast_bolus_flow_duration, contrast_bolus_ingredient,
     * contrast_bolus_ingredient_concentration.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& _previous,
        const ::fwAtoms::Object::sptr& _current,
        ::fwAtomsPatch::IPatch::NewVersionsType& _newVersions) override;

};

} // namespace ImageSeries
} // namespace sight::data
} // namespace fwStructuralPatch
