/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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
namespace fwMedData
{
namespace ModelSeries
{

/// Structural patch to convert a fwMedData::ModelSeries from version '2' to version '1'.
class FWSTRUCTURALPATCH_CLASS_API V2ToV1 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassMacro(V2ToV1, ::fwStructuralPatch::fwMedData::ModelSeries::V2ToV1, new V2ToV1);

    /// Constructor
    FWSTRUCTURALPATCH_API V2ToV1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V2ToV1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V2ToV1( const V2ToV1& cpy );

    /**
     * @brief Applies patch
     *
     * Removed attribute dicom_reference.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace ModelSeries
} // namespace fwMedData
} // namespace fwStructuralPatch
