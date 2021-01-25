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

#include "fwStructuralPatch/data/Mesh/V4ToV3.hpp"

#include <atoms/Blob.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <fwAtomsPatch/StructuralCreatorDB.hpp>

namespace fwStructuralPatch
{

namespace sight::data
{

namespace Mesh
{

V4ToV3::V4ToV3() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::sight::data::Mesh";
    m_targetClassname = "::sight::data::Mesh";
    m_originVersion   = "4";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V4ToV3::~V4ToV3()
{
}

// ----------------------------------------------------------------------------

V4ToV3::V4ToV3( const V4ToV3& cpy ) :
    ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V4ToV3::apply( const atoms::Object::sptr& previous,
                    const atoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

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
              atoms::String::dynamicCast((*currentStrides)[0])->setValue(std::to_string(sizeof(std::uint64_t)));

              // Set the new type.
              currentType->setValue("uint64");

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
              bufferMemory->allocate(size * sizeof(std::uint64_t));

              std::uint32_t* buff32 = static_cast<std::uint32_t*>( buff );
              std::uint64_t* buff64 = static_cast<std::uint64_t*>( bufferMemory->getBuffer() );

              // Iterate over buffers, and fill it with 32 bit values.
              for(size_t i = 0; i < size; ++i)
              {
                  buff64[i] = static_cast<std::uint64_t>(buff32[i]);
              }

              // Give the new buffer to atom.
              currentCellDataBuffer->setBufferObject(bufferMemory);
          };

    reformatBuffer("cell_data");
    reformatBuffer("cell_data_offsets");

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);
    helper.removeAttribute("attributes");
}

} // namespace Mesh

} // namespace sight::data

} // namespace fwStructuralPatch
