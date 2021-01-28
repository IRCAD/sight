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

#include <io/atoms/patch/IStructuralPatch.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace Image
{

/// Structural patch to convert a data::Image from version '2' to version '3'.
class IO_PATCH_CLASS_API V2ToV3 : public io::atoms::patch::IStructuralPatch
{
public:
    fwCoreClassMacro(V2ToV3, patch::structural::data::Image::V2ToV3, new V2ToV3)

    /// Constructor
    IO_PATCH_API V2ToV3();

    /// Destructor
    IO_PATCH_API virtual ~V2ToV3() override;

    /// Copy constructor
    IO_PATCH_API V2ToV3( const V2ToV3& cpy );

    /**
     * @brief Applies patch
     *
     * Removes attributes from reconstruction.
     */
    IO_PATCH_API virtual void apply(
        const sight::atoms::Object::sptr& previous,
        const sight::atoms::Object::sptr& current,
        io::atoms::patch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace Image

} // namespace data

} // namespace sight::io::patch::structural
