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

#include "fwStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralCreator.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace sight::data
{

/// Structural creator for data::PointList version '1'.
class FWSTRUCTURALPATCH_CLASS_API PointList1 : public ::fwAtomsPatch::IStructuralCreator
{
public:
    fwCoreClassMacro(PointList1, ::fwAtomsPatch::IStructuralCreator, new PointList1)

    /// Constructor
    FWSTRUCTURALPATCH_API PointList1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~PointList1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API PointList1( const PointList1& cpy );

    /// Create the specified object (sets 'points' attribute).
    FWSTRUCTURALPATCH_API virtual atoms::Object::sptr create() override;

};

} // namespace sight::data
} // namespace creator
} // namespace fwStructuralPatch
