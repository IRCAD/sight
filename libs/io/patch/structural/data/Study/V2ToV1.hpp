/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "io/patch/config.hpp"

#include <io/atoms/patch/IStructuralPatch.hpp>

namespace sight::io::patch::structural
{
namespace data
{
namespace Study
{

/// Structural patch to convert a data::Study from version '2' to version '1'.
class IO_PATCH_CLASS_API V2ToV1 : public io::atoms::patch::IStructuralPatch
{

public:

    SIGHT_DECLARE_CLASS(V2ToV1, patch::structural::data::Study::V2ToV1, new V2ToV1)

    /// Initialiaze members.
    IO_PATCH_API V2ToV1();

    /// Deletes resources.
    IO_PATCH_API ~V2ToV1();

    /**
     * @brief Copies the patch.
     * @param _cpy the patch to copy.
     */
    IO_PATCH_API V2ToV1(const V2ToV1& _cpy);

    /**
     * @brief Applies patch
     *
     * Removes attributes tudy_id, consulting_physician_name, patient_size, patient_weight, patient_body_mass_index.
     */
    IO_PATCH_API virtual void apply(
        const sight::atoms::Object::sptr& _previous,
        const sight::atoms::Object::sptr& _current,
        io::atoms::patch::IPatch::NewVersionsType& _newVersions) override;

};

} // namespace Study
} // namespace data
} // namespace sight::io::patch::structural
