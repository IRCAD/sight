/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/patch/config.hpp"

#include <io/atoms/patch/ISemanticPatch.hpp>

namespace sight::io::patch::semantic
{

namespace V1
{

namespace V2
{

namespace data
{

/// Patch the composite describing a 'Medical workspace' from version 'V1' to 'V2' within 'MedicalData' context.
class IO_PATCH_CLASS_API Composite : public io::atoms::patch::ISemanticPatch
{
public:

    SIGHT_DECLARE_CLASS(Composite, io::atoms::patch::ISemanticPatch, new Composite);

    /// Constructor
    IO_PATCH_API Composite();

    /// Destructor
    IO_PATCH_API ~Composite();

    /// Copy constructor
    IO_PATCH_API Composite(const Composite& cpy);

    /**
     * @brief Applies patch
     *
     * If the related composite describes a 'Medical workspace' (i.e has value 'MedicalWorkspace' for 'compositeType'
     * meta info), then converts the composite to a data::SeriesDB and insert model series built from previous
     * acquisitions.
     */
    IO_PATCH_API virtual void apply(
        const sight::atoms::Object::sptr& previous,
        const sight::atoms::Object::sptr& current,
        io::atoms::patch::IPatch::NewVersionsType& newVersions
    ) override;
};

} // namespace data

} // namespace V2

} // namespace V1

} // namespace sight::io::patch::semantic
