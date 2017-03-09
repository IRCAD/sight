/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions);

};

} // namespace fwData
} // namespace V10
} // namespace V09ALA
} // namespace fwMDSemanticPatch

#endif /* __FWMDSEMANTICPATCH_V09ALA_V10_FWDATA_IMAGE_HPP__ */

