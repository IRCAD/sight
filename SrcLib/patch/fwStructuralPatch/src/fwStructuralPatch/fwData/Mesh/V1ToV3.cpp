/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/String.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>

#include "fwStructuralPatch/fwData/Mesh/V1ToV3.hpp"

namespace fwStructuralPatch
{

namespace fwData
{

namespace Mesh
{

V1ToV3::V1ToV3() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Mesh";
    m_targetClassname = "::fwData::Mesh";
    m_originVersion   = "1";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V1ToV3::~V1ToV3()
{
}

// ----------------------------------------------------------------------------

V1ToV3::V1ToV3( const V1ToV3 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV3::apply( const ::fwAtoms::Object::sptr& previous,
                    const ::fwAtoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);
    helper.addAttribute("point_tex_coords", ::fwAtoms::Object::sptr() );
    helper.addAttribute("cell_tex_coords", ::fwAtoms::Object::sptr() );
    helper.addAttribute("array_map", ::fwAtoms::Map::New() );
}

} // namespace Mesh

} // namespace fwData

} // namespace fwStructuralPatch

