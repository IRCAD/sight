/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

/// Structural patch to convert a data::ImageSeries from version '1' to version '2'.
class IO_PATCH_CLASS_API V1ToV2 : public io::atoms::patch::IStructuralPatch
{
public:

    SIGHT_DECLARE_CLASS(V1ToV2, patch::structural::data::ImageSeries::V1ToV2, new V1ToV2);

    /// Constructor
    IO_PATCH_API V1ToV2();

    /// Destructor
    IO_PATCH_API ~V1ToV2();

    /// Copy constructor
    IO_PATCH_API V1ToV2(const V1ToV2& cpy);

    /**
     * @brief Applies patch
     *
     * Added new attribute dicom_reference.
     */
    IO_PATCH_API virtual void apply(
        const sight::atoms::Object::sptr& previous,
        const sight::atoms::Object::sptr& current,
        io::atoms::patch::IPatch::NewVersionsType& newVersions
    ) override;
};

} // namespace ImageSeries

} // namespace data

} // namespace sight::io::patch::structural
