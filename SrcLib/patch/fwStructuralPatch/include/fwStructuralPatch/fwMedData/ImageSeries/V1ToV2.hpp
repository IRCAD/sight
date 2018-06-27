/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace fwStructuralPatch
{
namespace fwMedData
{
namespace ImageSeries
{

/// Structural patch to convert a fwMedData::ImageSeries from version '1' to version '2'.
class FWSTRUCTURALPATCH_CLASS_API V1ToV2 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V1ToV2)(::fwStructuralPatch::fwMedData::ImageSeries::V1ToV2), (()), new V1ToV2);

    /// Constructor
    FWSTRUCTURALPATCH_API V1ToV2();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V1ToV2();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V1ToV2( const V1ToV2& cpy );

    /**
     * @brief Applies patch
     *
     * Added new attribute dicom_reference.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace ImageSeries
} // namespace fwMedData
} // namespace fwStructuralPatch
