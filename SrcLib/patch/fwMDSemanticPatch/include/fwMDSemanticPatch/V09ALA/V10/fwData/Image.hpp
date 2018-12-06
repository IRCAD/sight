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

#ifndef __FWMDSEMANTICPATCH_V09ALA_V10_FWDATA_IMAGE_HPP__
#define __FWMDSEMANTICPATCH_V09ALA_V10_FWDATA_IMAGE_HPP__

#include "fwMDSemanticPatch/config.hpp"

#include <fwAtomsPatch/ISemanticPatch.hpp>

namespace fwMDSemanticPatch
{
namespace V09ALA
{
namespace V10
{
namespace fwData
{

/// Patch the 'landmarks' field of an image from version 'V9ALA' to 'V10' within 'MedicalData' context.
class FWMDSEMANTICPATCH_CLASS_API Image : public ::fwAtomsPatch::ISemanticPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (Image)(::fwAtomsPatch::ISemanticPatch), (()), new Image);

    /// Constructor
    FWMDSEMANTICPATCH_API Image();

    /// Destructor
    FWMDSEMANTICPATCH_API ~Image();

    /// Copy constructor
    FWMDSEMANTICPATCH_API Image( const Image& cpy );

    /**
     * @brief Applies patch.
     *
     * Converts the "m_imageLandmarksId" field from a ::fwData::PointList to a ::fwData::Landmarks in "m_landmarksId" .
     */
    FWMDSEMANTICPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace fwData
} // namespace V10
} // namespace V09ALA
} // namespace fwMDSemanticPatch

#endif /* __FWMDSEMANTICPATCH_V09ALA_V10_FWDATA_IMAGE_HPP__ */

