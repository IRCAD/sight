/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_FWDATA_TRANSFORMATIONMATRIX3D_V1TOV2_HPP__
#define __FWSTRUCTURALPATCH_FWDATA_TRANSFORMATIONMATRIX3D_V1TOV2_HPP__

#include "fwStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace fwStructuralPatch
{

namespace fwData
{

namespace TransformationMatrix3D
{

/// Structural patch to convert a fwData::Mesh from version '1' to version '2'.
class FWSTRUCTURALPATCH_CLASS_API V1ToV2 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V1ToV2)(::fwStructuralPatch::fwData::TransformationMatrix3D::V1ToV2), (()), new V1ToV2);

    /// Constructor
    FWSTRUCTURALPATCH_API V1ToV2();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V1ToV2();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V1ToV2( const V1ToV2& cpy );

    /**
     * @brief Applies patch
     *
     * Added new map of additional array.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace TransformationMatrix3D

} // namespace fwData

} // namespace fwStructuralPatch

#endif /* __FWSTRUCTURALPATCH_FWDATA_TRANSFORMATIONMATRIX3D_V1TOV2_HPP__ */

