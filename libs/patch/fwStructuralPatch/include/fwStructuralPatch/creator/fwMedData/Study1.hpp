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

#include <fwAtomsPatch/IStructuralCreator.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

/// Structural creator for ::fwMedData::Study version '1'.
class FWSTRUCTURALPATCH_CLASS_API Study1 : public ::fwAtomsPatch::IStructuralCreator
{

public:

    fwCoreClassMacro(Study1, ::fwAtomsPatch::IStructuralCreator, new Study1);

    /// Constructor
    FWSTRUCTURALPATCH_API Study1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~Study1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API Study1( const Study1& cpy );

    /**
     * @brief Creates the specified object.
     *
     * Sets 'instance_uid', 'date', 'time', 'referring_physician_name', 'description' and 'patient_age' attributes.
     */
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create() override;

};

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
