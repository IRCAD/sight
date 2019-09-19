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
#include "fwStructuralPatch/creator/fwMedData/Series1.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

/// Structural creator for ::fwMedData::ModelSeries version '1'.
class FWSTRUCTURALPATCH_CLASS_API ModelSeries1 : public ::fwStructuralPatch::creator::fwMedData::Series1
{

public:

    fwCoreClassMacro(ModelSeries1, ::fwStructuralPatch::creator::fwMedData::Series1, new ModelSeries1);

    /// Constructor
    FWSTRUCTURALPATCH_API ModelSeries1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~ModelSeries1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API ModelSeries1( const ModelSeries1& cpy );

    /// Creates the specified object (sets 'reconstruction_db' attribute) and calls creator 'Series1'.
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create() override;

};

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
