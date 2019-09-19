/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace fwStructuralPatch
{

namespace fwData
{

namespace TransformationMatrix3D
{

/// Structural patch to convert a fwData::Mesh from version '1' to version '2'.
class FWSTRUCTURALPATCH_CLASS_API V1ToV2 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassMacro(V1ToV2, ::fwStructuralPatch::fwData::TransformationMatrix3D::V1ToV2, new V1ToV2);

    /// Constructor
    FWSTRUCTURALPATCH_API V1ToV2();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V1ToV2();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V1ToV2( const V1ToV2& cpy );

    /**
     * @brief Applies patch
     *
     * Added new map of additional array.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace TransformationMatrix3D

} // namespace fwData

} // namespace fwStructuralPatch
