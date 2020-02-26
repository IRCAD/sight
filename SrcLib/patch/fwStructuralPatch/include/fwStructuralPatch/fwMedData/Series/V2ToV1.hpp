/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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
namespace fwMedData
{
namespace Series
{

/// Structural patch to convert a fwMedData::Series from version '2' to version '1'.
class FWSTRUCTURALPATCH_CLASS_API V2ToV1 : public ::fwAtomsPatch::IStructuralPatch
{

public:

    fwCoreClassMacro(V2ToV1, ::fwStructuralPatch::fwMedData::Series::V2ToV1, new V2ToV1)

    /// Initialiaze members.
    FWSTRUCTURALPATCH_API V2ToV1();

    /// Deletes resources.
    FWSTRUCTURALPATCH_API ~V2ToV1();

    /**
     * @brief Copies the patch.
     * @param _cpy the patch to copy.
     */
    FWSTRUCTURALPATCH_API V2ToV1(const V2ToV1& _cpy);

    /**
     * @brief Applies patch
     *
     * Removes attributes instance_uid, number, laterality, protocolName, body_part_examined, patient_position,
     * anatomical_orientation_type, performded_procedure_step_id, performed_procedure_step_start_date,
     * performed_procedure_step_start_time, performed_procedure_step_end_date, performed_procedure_step_end_time,
     * performed_procedure_step_description, performed_procedure_comments.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& _previous,
        const ::fwAtoms::Object::sptr& _current,
        ::fwAtomsPatch::IPatch::NewVersionsType& _newVersions) override;

};

} // namespace Series
} // namespace fwMedData
} // namespace fwStructuralPatch
