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
namespace ModelSeries
{

/// Structural patch to convert a fwMedData::ModelSeries from version '2' to version '1'.
class FWSTRUCTURALPATCH_CLASS_API V2ToV1 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V2ToV1)(::fwStructuralPatch::fwMedData::ModelSeries::V2ToV1), (()), new V2ToV1);

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
