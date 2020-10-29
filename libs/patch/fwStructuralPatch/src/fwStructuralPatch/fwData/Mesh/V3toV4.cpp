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
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

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

    // Convert cell_data array from std::uint64_t to std::uint32_t
    auto reformatBuffer
        = [&](const std::string& _buffer)
          {
              // Retrieves values from previous atoms object.
              ::fwAtoms::Object::sptr cellData     = previous->getAttribute< ::fwAtoms::Object >(_buffer);
              ::fwAtoms::Blob::sptr cellDataBuffer = cellData->getAttribute< ::fwAtoms::Blob >("buffer");
              ::fwAtoms::Sequence::sptr strides    = cellData->getAttribute< ::fwAtoms::Sequence >("strides");
              ::fwAtoms::String::sptr stride       = ::fwAtoms::String::dynamicCast((*strides)[0]);
              size_t cellDataElementSize = static_cast< size_t >(stoi(stride->getString()));

              // Retrieves values from current atoms object.
              ::fwAtoms::Object::sptr currentCellData     = current->getAttribute< ::fwAtoms::Object >(_buffer);
              ::fwAtoms::Blob::sptr currentCellDataBuffer = currentCellData->getAttribute< ::fwAtoms::Blob >("buffer");
              ::fwAtoms::Sequence::sptr currentStrides    = currentCellData->getAttribute< ::fwAtoms::Sequence >(
                  "strides");
              ::fwAtoms::String::sptr currentType = currentCellData->getAttribute< ::fwAtoms::String >("type");

              // Set the new strides to sizeof(std::uint32_t).
              ::fwAtoms::String::dynamicCast((*currentStrides)[0])->setValue(std::to_string(sizeof(std::uint32_t)));

              // Set the new type.
              currentType->setValue("uint32");

              // Get buffers, and fill current one with uint32_t values.
              const auto bo     = cellDataBuffer->getBufferObject();
              const auto lock   = bo->lock();
              const auto buff   = bo->getBuffer();
              const auto bosize = bo->getSize();

              // Divide the size in bytes by the size of one element to get the number of elements.
              const size_t size = bosize / cellDataElementSize;

              // Create a new buffer object with a size of size*32.
              ::fwMemory::BufferObject::sptr bufferMemory = ::fwMemory::BufferObject::New();
              const auto bufferMemoryLock = bufferMemory->lock();
              bufferMemory->allocate(size * sizeof(std::uint32_t));

              std::uint64_t* buff64 = static_cast<std::uint64_t*>( buff );
              std::uint32_t* buff32 = static_cast<std::uint32_t*>( bufferMemory->getBuffer() );

              // Iterate over buffers, and fill it with 32 bit values.
              for(size_t i = 0; i < size; ++i)
              {
                  buff32[i] = static_cast<std::uint32_t>(buff64[i]);
              }

              // Give the new buffer to atom.
              currentCellDataBuffer->setBufferObject(bufferMemory);
          };

    reformatBuffer("cell_data");
    reformatBuffer("cell_data_offsets");

    ::fwAtoms::Object::sptr pointC = previous->getAttribute< ::fwAtoms::Object >("point_colors");
    if(pointC)
    {
        ::fwAtoms::Blob::sptr pointCBuffer = pointC->getAttribute< ::fwAtoms::Blob >("buffer");

        if(pointCBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::POINT_COLORS;
        }
    }

    ::fwAtoms::Object::sptr pointN = previous->getAttribute< ::fwAtoms::Object >("point_normals");
    if(pointN)
    {
        ::fwAtoms::Blob::sptr pointNBuffer = pointN->getAttribute< ::fwAtoms::Blob >("buffer");

        if(pointNBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::POINT_NORMALS;
        }
    }

    ::fwAtoms::Object::sptr pointT = previous->getAttribute< ::fwAtoms::Object >("point_tex_coords");
    if(pointT)
    {
        ::fwAtoms::Blob::sptr pointTBuffer = pointT->getAttribute< ::fwAtoms::Blob >("buffer");

        if(pointTBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::POINT_TEX_COORDS;
        }
    }

    ::fwAtoms::Object::sptr cellC = previous->getAttribute< ::fwAtoms::Object >("cell_colors");
    if(cellC)
    {
        ::fwAtoms::Blob::sptr cellCBuffer = cellC->getAttribute< ::fwAtoms::Blob >("buffer");

        if(cellCBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::CELL_COLORS;
        }
    }
    ::fwAtoms::Object::sptr cellN = previous->getAttribute< ::fwAtoms::Object >("cell_normals");
    if(cellN)
    {
        ::fwAtoms::Blob::sptr cellNBuffer = cellN->getAttribute< ::fwAtoms::Blob >("buffer");

        if(cellNBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::CELL_NORMALS;
        }
    }

    ::fwAtoms::Object::sptr cellT = previous->getAttribute< ::fwAtoms::Object >("cell_tex_coords");
    if(cellT)
    {
        ::fwAtoms::Blob::sptr cellTBuffer = cellT->getAttribute< ::fwAtoms::Blob >("buffer");

        if(cellTBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | ::fwData::Mesh::Attributes::CELL_TEX_COORDS;
        }
    }

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);
    helper.addAttribute("attributes", ::fwAtoms::Numeric::New<int>(static_cast<int>(meshAttributes)) );
}

} // namespace Mesh

} // namespace fwData

} // namespace fwStructuralPatch
