/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARSTRUCTURALPATCH_ARDATA_CAMERA_V3TOV2_HPP__
#define __ARSTRUCTURALPATCH_ARDATA_CAMERA_V3TOV2_HPP__

#include "arStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace arStructuralPatch
{

namespace arData
{

namespace Camera
{

/// Structural patch to convert a arData::Camera from version '3' to version '2'.
class ARSTRUCTURALPATCH_CLASS_API V3ToV2 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V3ToV2)(::arStructuralPatch::arData::Camera::V3ToV2), (()), new V3ToV2);

    /// Constructor
    ARSTRUCTURALPATCH_API V3ToV2();

    /// Destructor
    ARSTRUCTURALPATCH_API ~V3ToV2();

    /// Copy constructor
    ARSTRUCTURALPATCH_API V3ToV2( const V3ToV2& cpy );

    /**
     * @brief Applies patch
     *
     * Removes scale attribute.
     */
    ARSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions);

};

} // namespace Camera

} // namespace arData

} // namespace arStructuralPatch

#endif /* __ARSTRUCTURALPATCH_ARDATA_CAMERA_V3TOV2_HPP__ */

