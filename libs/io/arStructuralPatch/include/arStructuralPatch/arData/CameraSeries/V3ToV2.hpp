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

#include "arStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace arStructuralPatch
{
namespace sight::data
{
namespace CameraSeries
{

/// Structural patch to convert a data::CameraSeries from version '3' to version '2'.
class ARSTRUCTURALPATCH_CLASS_API V3ToV2 : public ::fwAtomsPatch::IStructuralPatch
{

public:

    fwCoreClassMacro(V3ToV2, ::arStructuralPatchdata::CameraSeries::V3ToV2, new V3ToV2)

    /// Initialiaze members.
    ARSTRUCTURALPATCH_API V3ToV2();

    /// Deletes resources.
    ARSTRUCTURALPATCH_API ~V3ToV2();

    /**
     * @brief Copies the patch.
     * @param _cpy the patch to copy.
     */
    ARSTRUCTURALPATCH_API V3ToV2( const V3ToV2& cpy );

    /**
     * @brief Applies patch
     *
     * Removes attributes instance_uid, number, laterality, protocolName, body_part_examined, patient_position,
     * anatomical_orientation_type, performded_procedure_step_id, performed_procedure_step_start_date,
     * performed_procedure_step_start_time, performed_procedure_step_end_date, performed_procedure_step_end_time,
     * performed_procedure_step_description, performed_procedure_comments.
     */
    ARSTRUCTURALPATCH_API virtual void apply(
        const atoms::Object::sptr& _previous,
        const atoms::Object::sptr& _current,
        ::fwAtomsPatch::IPatch::NewVersionsType& _newVersions) override;

};

} // namespace CameraSeries
} // namespace sight::data
} // namespace arStructuralPatch
