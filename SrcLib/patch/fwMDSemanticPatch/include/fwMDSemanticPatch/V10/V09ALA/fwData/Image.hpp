/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMDSEMANTICPATCH_V10_V09ALA_FWDATA_IMAGE_HPP__
#define __FWMDSEMANTICPATCH_V10_V09ALA_FWDATA_IMAGE_HPP__

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

#endif /* __FWMDSEMANTICPATCH_V10_V09ALA_FWDATA_IMAGE_HPP__ */

