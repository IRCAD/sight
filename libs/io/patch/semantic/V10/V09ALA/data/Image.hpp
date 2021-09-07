/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <io/atoms/patch/ISemanticPatch.hpp>

namespace sight::io::patch::semantic
{

namespace V10
{

namespace V09ALA
{

namespace data
{

/// Patch the 'landmarks' field of an image from version 'V10' to 'V09ALA' within 'MedicalData' context.
class IO_PATCH_CLASS_API Image : public io::atoms::patch::ISemanticPatch
{
public:

    SIGHT_DECLARE_CLASS(Image, io::atoms::patch::ISemanticPatch, new Image);

    /// Constructor
    IO_PATCH_API Image();

    /// Destructor
    IO_PATCH_API ~Image();

    /// Copy constructor
    IO_PATCH_API Image(const Image& cpy);

    /**
     * @brief Applies patch.
     *
     * Converts the "m_landmarksId" field from a data::Landmarks to a data::PointList in "m_imageLandmarksId" .
     */
    IO_PATCH_API virtual void apply(
        const sight::atoms::Object::sptr& previous,
        const sight::atoms::Object::sptr& current,
        io::atoms::patch::IPatch::NewVersionsType& newVersions
    ) override;
};

} // namespace data

} // namespace V09ALA

} // namespace V10

} // namespace sight::io::patch::semantic
