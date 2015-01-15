/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/String.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>

#include "fwStructuralPatch/fwData/Mesh/V2ToV3.hpp"

namespace fwStructuralPatch
{

namespace fwData
{

namespace Mesh
{

V2ToV3::V2ToV3() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Mesh";
    m_targetClassname = "::fwData::Mesh";
    m_originVersion = "2";
    m_targetVersion = "3";
}

// ----------------------------------------------------------------------------

V2ToV3::~V2ToV3()
{
}

// ----------------------------------------------------------------------------

V2ToV3::V2ToV3( const V2ToV3 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V2ToV3::apply( const ::fwAtoms::Object::sptr& previous,
                    const ::fwAtoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);
    if (current->getAttributes().find("point_tex_coords") == current->getAttributes().end())
    {
        helper.addAttribute("point_tex_coords", ::fwAtoms::Object::sptr() );
    }
    if (current->getAttributes().find("cell_tex_coords") == current->getAttributes().end())
    {
        helper.addAttribute("cell_tex_coords", ::fwAtoms::Object::sptr() );
    }
}

} // namespace Mesh

} // namespace fwData

} // namespace fwStructuralPatch

