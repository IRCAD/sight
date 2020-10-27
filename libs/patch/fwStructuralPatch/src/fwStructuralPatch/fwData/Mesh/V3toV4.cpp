/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwStructuralPatch/fwData/Mesh/V3ToV4.hpp"

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>

#include <fwAtomsPatch/StructuralCreatorDB.hpp>

#include <fwData/Mesh.hpp>

#include <fwMemory/BufferObject.hpp>

namespace fwStructuralPatch
{

namespace fwData
{

namespace Mesh
{

V3ToV4::V3ToV4() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Mesh";
    m_targetClassname = "::fwData::Mesh";
    m_originVersion   = "3";
    m_targetVersion   = "4";
}

// ----------------------------------------------------------------------------

V3ToV4::~V3ToV4()
{
}

// ----------------------------------------------------------------------------

V3ToV4::V3ToV4( const V3ToV4& cpy ) :
    ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V3ToV4::apply( const ::fwAtoms::Object::sptr& previous,
                    const ::fwAtoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    ::fwData::Mesh::Attributes meshAttributes = ::fwData::Mesh::Attributes::NONE;

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    ::fwAtoms::Object::sptr pointC     = previous->getAttribute< ::fwAtoms::Object >("point_colors");
    ::fwAtoms::Blob::sptr pointCBuffer = pointC->getAttribute< ::fwAtoms::Blob >("buffer");

    if(pointCBuffer->getBufferObject()->getBuffer())
    {
        meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::POINT_COLORS;
    }

    ::fwAtoms::Object::sptr pointN     = previous->getAttribute< ::fwAtoms::Object >("point_normals");
    ::fwAtoms::Blob::sptr pointNBuffer = pointN->getAttribute< ::fwAtoms::Blob >("buffer");

    if(pointNBuffer->getBufferObject()->getBuffer())
    {
        meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::POINT_NORMALS;
    }

    ::fwAtoms::Object::sptr pointT     = previous->getAttribute< ::fwAtoms::Object >("point_tex_coords");
    ::fwAtoms::Blob::sptr pointTBuffer = pointT->getAttribute< ::fwAtoms::Blob >("buffer");

    if(pointTBuffer->getBufferObject()->getBuffer())
    {
        meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::POINT_TEX_COORDS;
    }

    ::fwAtoms::Object::sptr cellC     = previous->getAttribute< ::fwAtoms::Object >("cell_colors");
    ::fwAtoms::Blob::sptr cellCBuffer = cellC->getAttribute< ::fwAtoms::Blob >("buffer");

    if(cellCBuffer->getBufferObject()->getBuffer())
    {
        meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::CELL_COLORS;
    }

    ::fwAtoms::Object::sptr cellN     = previous->getAttribute< ::fwAtoms::Object >("cell_normals");
    ::fwAtoms::Blob::sptr cellNBuffer = cellN->getAttribute< ::fwAtoms::Blob >("buffer");

    if(cellNBuffer->getBufferObject()->getBuffer())
    {
        meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::CELL_NORMALS;
    }

    ::fwAtoms::Object::sptr cellT     = previous->getAttribute< ::fwAtoms::Object >("cell_tex_coords");
    ::fwAtoms::Blob::sptr cellTBuffer = cellT->getAttribute< ::fwAtoms::Blob >("buffer");

    if(cellTBuffer->getBufferObject()->getBuffer())
    {
        meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::CELL_TEX_COORDS;
    }

    helper.addAttribute("attributes", ::fwAtoms::Numeric::New<int>(static_cast<int>(meshAttributes)) );

}

} // namespace Mesh

} // namespace fwData

} // namespace fwStructuralPatch
