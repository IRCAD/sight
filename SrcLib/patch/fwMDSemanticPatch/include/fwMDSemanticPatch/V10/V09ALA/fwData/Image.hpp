/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
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

#include "fwMDSemanticPatch/config.hpp"

#include <fwAtomsPatch/ISemanticPatch.hpp>

namespace fwMDSemanticPatch
{
namespace V10
{
namespace V09ALA
{
namespace fwData
{

/// Patch the 'landmarks' field of an image from version 'V10' to 'V09ALA' within 'MedicalData' context.
class FWMDSEMANTICPATCH_CLASS_API Image : public ::fwAtomsPatch::ISemanticPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((Image)(::fwAtomsPatch::ISemanticPatch), new Image);

    /// Constructor
    FWMDSEMANTICPATCH_API Image();

    /// Destructor
    FWMDSEMANTICPATCH_API ~Image();

    /// Copy constructor
    FWMDSEMANTICPATCH_API Image( const Image& cpy );

    /**
     * @brief Applies patch.
     *
     * Converts the "m_landmarksId" field from a ::fwData::Landmarks to a ::fwData::PointList in "m_imageLandmarksId" .
     */
    FWMDSEMANTICPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace fwData
} // namespace V09ALA
} // namespace V10
} // namespace fwMDSemanticPatch
