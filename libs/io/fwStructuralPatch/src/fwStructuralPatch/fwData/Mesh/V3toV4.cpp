/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "fwStructuralPatch/data/Mesh/V3ToV4.hpp"

#include <atoms/Blob.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/memory/BufferObject.hpp>

#include <data/Mesh.hpp>

#include <fwAtomsPatch/StructuralCreatorDB.hpp>

namespace fwStructuralPatch
{

namespace sight::data
{

namespace Mesh
{

V3ToV4::V3ToV4() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::sight::data::Mesh";
    m_targetClassname = "::sight::data::Mesh";
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

void V3ToV4::apply( const atoms::Object::sptr& previous,
                    const atoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    data::Mesh::Attributes meshAttributes = data::Mesh::Attributes::NONE;

    // Convert cell_data array from std::uint64_t to std::uint32_t
    auto reformatBuffer
        = [&](const std::string& _buffer)
          {
              // Retrieves values from previous atoms object.
              atoms::Object::sptr cellData     = previous->getAttribute< atoms::Object >(_buffer);
              atoms::Blob::sptr cellDataBuffer = cellData->getAttribute< atoms::Blob >("buffer");
              atoms::Sequence::sptr strides    = cellData->getAttribute< atoms::Sequence >("strides");
              atoms::String::sptr stride       = atoms::String::dynamicCast((*strides)[0]);
              size_t cellDataElementSize       = static_cast< size_t >(stoi(stride->getString()));

              // Retrieves values from current atoms object.
              atoms::Object::sptr currentCellData     = current->getAttribute< atoms::Object >(_buffer);
              atoms::Blob::sptr currentCellDataBuffer = currentCellData->getAttribute< atoms::Blob >("buffer");
              atoms::Sequence::sptr currentStrides    = currentCellData->getAttribute< atoms::Sequence >(
                  "strides");
              atoms::String::sptr currentType = currentCellData->getAttribute< atoms::String >("type");

              // Set the new strides to sizeof(std::uint32_t).
              atoms::String::dynamicCast((*currentStrides)[0])->setValue(std::to_string(sizeof(std::uint32_t)));

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
              core::memory::BufferObject::sptr bufferMemory = core::memory::BufferObject::New();
              const auto bufferMemoryLock                   = bufferMemory->lock();
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

    atoms::Object::sptr pointC = previous->getAttribute< atoms::Object >("point_colors");
    if(pointC)
    {
        atoms::Blob::sptr pointCBuffer = pointC->getAttribute< atoms::Blob >("buffer");

        if(pointCBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | data::Mesh::Attributes::POINT_COLORS;
        }
    }

    atoms::Object::sptr pointN = previous->getAttribute< atoms::Object >("point_normals");
    if(pointN)
    {
        atoms::Blob::sptr pointNBuffer = pointN->getAttribute< atoms::Blob >("buffer");

        if(pointNBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | data::Mesh::Attributes::POINT_NORMALS;
        }
    }

    atoms::Object::sptr pointT = previous->getAttribute< atoms::Object >("point_tex_coords");
    if(pointT)
    {
        atoms::Blob::sptr pointTBuffer = pointT->getAttribute< atoms::Blob >("buffer");

        if(pointTBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | data::Mesh::Attributes::POINT_TEX_COORDS;
        }
    }

    atoms::Object::sptr cellC = previous->getAttribute< atoms::Object >("cell_colors");
    if(cellC)
    {
        atoms::Blob::sptr cellCBuffer = cellC->getAttribute< atoms::Blob >("buffer");

        if(cellCBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | data::Mesh::Attributes::CELL_COLORS;
        }
    }
    atoms::Object::sptr cellN = previous->getAttribute< atoms::Object >("cell_normals");
    if(cellN)
    {
        atoms::Blob::sptr cellNBuffer = cellN->getAttribute< atoms::Blob >("buffer");

        if(cellNBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | data::Mesh::Attributes::CELL_NORMALS;
        }
    }

    atoms::Object::sptr cellT = previous->getAttribute< atoms::Object >("cell_tex_coords");
    if(cellT)
    {
        atoms::Blob::sptr cellTBuffer = cellT->getAttribute< atoms::Blob >("buffer");

        if(cellTBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | data::Mesh::Attributes::CELL_TEX_COORDS;
        }
    }

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);
    helper.addAttribute("attributes", atoms::Numeric::New<int>(static_cast<int>(meshAttributes)) );
}

} // namespace Mesh

} // namespace sight::data

} // namespace fwStructuralPatch
