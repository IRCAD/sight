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

namespace Study
{

/// Transforms a ::fwData::Study to a ::fwMedData::Study.
class FWSTRUCTURALPATCH_CLASS_API V1ToFwMedDataStudyV1 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V1ToFwMedDataStudyV1)(::fwStructuralPatch::fwData::Study::V1ToFwMedDataStudyV1),
        new V1ToFwMedDataStudyV1);

    /// Constructor
    FWSTRUCTURALPATCH_API V1ToFwMedDataStudyV1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V1ToFwMedDataStudyV1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V1ToFwMedDataStudyV1( const V1ToFwMedDataStudyV1& cpy );

    /**
     * @brief Applies patch
     *
     * Removes attributes 'hospital', 'modality', 'acquisition_zone', 'ris_id', 'db_id' and 'acquisitions', renames
     * 'uid' to 'instance_uid', adds 'referring_physicians_name' and 'patient_age'.
     */
    FWSTRUCTURALPATCH_API virtual void apply(const ::fwAtoms::Object::sptr& previous,
                                             const ::fwAtoms::Object::sptr& current,
                                             ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace Study

} // namespace fwData

} // namespace fwStructuralPatch
