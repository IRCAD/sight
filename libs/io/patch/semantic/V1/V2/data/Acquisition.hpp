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

/// Patch an acquisition from version 'V1' to 'V2' within 'MedicalData' context.
class IO_PATCH_CLASS_API Acquisition : public io::atoms::patch::ISemanticPatch
{
public:
    fwCoreClassMacro(Acquisition, io::atoms::patch::ISemanticPatch, new Acquisition)

    /// Constructor
    IO_PATCH_API Acquisition();

    /// Destructor
    IO_PATCH_API ~Acquisition();

    /// Copy constructor
    IO_PATCH_API Acquisition( const Acquisition& cpy );

    /**
     * @brief Applies patch
     *
     * Retrieves 'comment' field stored into acquisition's image and moves it into current object 'description'
     * attribute.
     */
    IO_PATCH_API virtual void apply(
        const sight::atoms::Object::sptr& previous,
        const sight::atoms::Object::sptr& current,
        io::atoms::patch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace data
} // namespace V2
} // namespace V1
} // namespace sight::io::patch::semantic
