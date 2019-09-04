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

#include "fwMDSemanticPatch/config.hpp"

#include <fwAtomsPatch/ISemanticPatch.hpp>

namespace fwMDSemanticPatch
{
namespace V1
{
namespace V2
{
namespace fwData
{

/// Patch the composite describing a 'Medical workspace' from version 'V1' to 'V2' within 'MedicalData' context.
class FWMDSEMANTICPATCH_CLASS_API Composite : public ::fwAtomsPatch::ISemanticPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((Composite)(::fwAtomsPatch::ISemanticPatch), new Composite);

    /// Constructor
    FWMDSEMANTICPATCH_API Composite();

    /// Destructor
    FWMDSEMANTICPATCH_API ~Composite();

    /// Copy constructor
    FWMDSEMANTICPATCH_API Composite( const Composite& cpy );

    /**
     * @brief Applies patch
     *
     * If the related composite describes a 'Medical workspace' (i.e has value 'MedicalWorkspace' for 'compositeType'
     * meta info), then converts the composite to a ::fwMedData::SeriesDB and insert model series built from previous
     * acquisitions.
     */
    FWMDSEMANTICPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace fwData
} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch
