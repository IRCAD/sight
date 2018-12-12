/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

/// Structural creator for ::fwMedData::Series version 1.
class FWSTRUCTURALPATCH_CLASS_API Series1 : public ::fwAtomsPatch::IStructuralCreator
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Series1)(::fwAtomsPatch::IStructuralCreator), (()), new Series1);

    /// Constructor
    FWSTRUCTURALPATCH_API Series1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~Series1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API Series1( const Series1& cpy );

    /**
     * @brief Creates the specified object.
     *
     * Calls creators for 'patient', 'study' and 'equipment' attributes and sets remaining attributes.
     */
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create() override;

};

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
