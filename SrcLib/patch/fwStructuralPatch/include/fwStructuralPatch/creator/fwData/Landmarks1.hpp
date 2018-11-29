/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include <fwAtomsPatch/IStructuralCreator.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace fwData
{

/// Structural creator for ::fwData::Landmarks version '1'.
class FWSTRUCTURALPATCH_CLASS_API Landmarks1 : public ::fwAtomsPatch::IStructuralCreator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Landmarks1)(::fwAtomsPatch::IStructuralCreator), (()), new Landmarks1);

    /// Constructor
    FWSTRUCTURALPATCH_API Landmarks1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~Landmarks1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API Landmarks1( const Landmarks1& cpy );

    /// Create the specified object (sets 'landmarks' attribute).
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create() override;

};

} // namespace fwData
} // namespace creator
} // namespace fwStructuralPatch
