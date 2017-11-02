/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_FWDATA_MATERIAL_V3TOV4_HPP__
#define __FWSTRUCTURALPATCH_FWDATA_MATERIAL_V3TOV4_HPP__

#include "fwStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace fwStructuralPatch
{

namespace fwData
{

namespace Material
{

/// Structural patch to convert a fwData::Material from version '3' to version '4'.
class FWSTRUCTURALPATCH_CLASS_API V3ToV4 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V3ToV4)(::fwStructuralPatch::fwData::Material::V3ToV4), (()), new V3ToV4);

    /// Constructor
    FWSTRUCTURALPATCH_API V3ToV4();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V3ToV4();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V3ToV4( const V3ToV4& cpy );

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

} // namespace Material

} // namespace fwData

} // namespace fwStructuralPatch

#endif /* __FWSTRUCTURALPATCH_FWDATA_MATERIAL_V3TOV4_HPP__ */
