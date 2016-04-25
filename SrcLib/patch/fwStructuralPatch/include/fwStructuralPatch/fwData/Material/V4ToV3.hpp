/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_FWDATA_MATERIAL_V4TOV3_HPP__
#define __FWSTRUCTURALPATCH_FWDATA_MATERIAL_V4TOV3_HPP__

#include "fwStructuralPatch/config.hpp"


#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace fwStructuralPatch
{

namespace fwData
{

namespace Material
{

/// Structural patch to convert a fwData::Material from version '4' to version '3'.
class FWSTRUCTURALPATCH_CLASS_API V4ToV3 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V4ToV3)(::fwStructuralPatch::fwData::Material::V4ToV3), (()), new V4ToV3);

    /// Constructor
    FWSTRUCTURALPATCH_API V4ToV3();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V4ToV3();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V4ToV3( const V4ToV3 &cpy );

    /**
     * @brief Applies patch
     *
     * Added new map of additional array.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions);

};


} // namespace Material

} // namespace fwData

} // namespace fwStructuralPatch

#endif /* __FWSTRUCTURALPATCH_FWDATA_MATERIAL_V4TOV3_HPP__ */
