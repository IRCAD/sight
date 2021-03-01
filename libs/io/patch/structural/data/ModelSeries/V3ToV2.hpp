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
namespace ModelSeries
{

/// Structural patch to convert a data::ModelSeries from version '3' to version '2'.
class IO_PATCH_CLASS_API V3ToV2 : public io::atoms::patch::IStructuralPatch
{

public:

    SIGHT_DECLARE_CLASS(V3ToV2, patch::structural::data::ModelSeries::V3ToV2, new V3ToV2)

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
     * @brief Applies patch
     *
     * Removes attributes instance_uid, number, laterality, protocolName, body_part_examined, patient_position,
     * anatomical_orientation_type, performded_procedure_step_id, performed_procedure_step_start_date,
     * performed_procedure_step_start_time, performed_procedure_step_end_date, performed_procedure_step_end_time,
     * performed_procedure_step_description, performed_procedure_comments.
     */
    IO_PATCH_API virtual void apply(
        const sight::atoms::Object::sptr& _previous,
        const sight::atoms::Object::sptr& _current,
        io::atoms::patch::IPatch::NewVersionsType& _newVersions) override;

};

} // namespace ModelSeries
} // namespace data
} // namespace sight::io::patch::structural
