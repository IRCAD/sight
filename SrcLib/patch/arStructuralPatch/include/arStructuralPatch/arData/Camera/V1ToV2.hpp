/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARSTRUCTURALPATCH_ARDATA_CAMERA_V1TOV2_HPP__
#define __ARSTRUCTURALPATCH_ARDATA_CAMERA_V1TOV2_HPP__

#include "arStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace arStructuralPatch
{

namespace arData
{

namespace Camera
{

/// Structural patch to convert a arData::Camera from version '1' to version '2'.
class ARSTRUCTURALPATCH_CLASS_API V1ToV2 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V1ToV2)(::arStructuralPatch::arData::Camera::V1ToV2), (()), new V1ToV2);

    /// Constructor
    ARSTRUCTURALPATCH_API V1ToV2();

    /// Destructor
    ARSTRUCTURALPATCH_API ~V1ToV2();

    /// Copy constructor
    ARSTRUCTURALPATCH_API V1ToV2( const V1ToV2& cpy );

    /**
     * @brief Applies patch
     *
     * Removes attributes from reconstruction.
     */
    ARSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace Camera

} // namespace arData

} // namespace arStructuralPatch

#endif /* __ARSTRUCTURALPATCH_ARDATA_CAMERA_V1TOV2_HPP__ */

