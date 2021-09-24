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

#include "io/patch/structural/data/Mesh/V3ToV4.hpp"

#include <atoms/Blob.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/memory/BufferObject.hpp>

#include <data/Mesh.hpp>

#include <io/atoms/patch/StructuralCreatorDB.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace Mesh
{

V3ToV4::V3ToV4() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "sight::data::Mesh";
    m_targetClassname = "sight::data::Mesh";
    m_originVersion   = "3";
    m_targetVersion   = "4";
}

// ----------------------------------------------------------------------------

V3ToV4::~V3ToV4()
{
}

// ----------------------------------------------------------------------------

V3ToV4::V3ToV4(const V3ToV4& cpy) :
    io::atoms::patch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V3ToV4::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    sight::data::Mesh::Attributes meshAttributes = sight::data::Mesh::Attributes::NONE;

    // Convert cell_data array from std::uint64_t to std::uint32_t
    auto reformatBuffer =
        [&](const std::string& _buffer)
        {
            // Retrieves values from previous atoms object.
            sight::atoms::Object::sptr cellData     = previous->getAttribute<sight::atoms::Object>(_buffer);
            sight::atoms::Blob::sptr cellDataBuffer = cellData->getAttribute<sight::atoms::Blob>("buffer");
            sight::atoms::Sequence::sptr strides    = cellData->getAttribute<sight::atoms::Sequence>("strides");
            sight::atoms::String::sptr stride       = sight::atoms::String::dynamicCast((*strides)[0]);
            size_t cellDataElementSize              = static_cast<size_t>(stoi(stride->getString()));

            // Retrieves values from current atoms object.
            sight::atoms::Object::sptr currentCellData     = current->getAttribute<sight::atoms::Object>(_buffer);
            sight::atoms::Blob::sptr currentCellDataBuffer = currentCellData->getAttribute<sight::atoms::Blob>(
                "buffer"
            );
            sight::atoms::Sequence::sptr currentStrides = currentCellData->getAttribute<sight::atoms::Sequence>(
                "strides"
            );
            sight::atoms::String::sptr currentType = currentCellData->getAttribute<sight::atoms::String>("type");

            // Set the new strides to sizeof(std::uint32_t).
            sight::atoms::String::dynamicCast((*currentStrides)[0])->setValue(std::to_string(sizeof(std::uint32_t)));

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

            std::uint64_t* buff64 = static_cast<std::uint64_t*>(buff);
            std::uint32_t* buff32 = static_cast<std::uint32_t*>(bufferMemory->getBuffer());

            // Iterate over buffers, and fill it with 32 bit values.
            for(size_t i = 0 ; i < size ; ++i)
            {
                buff32[i] = static_cast<std::uint32_t>(buff64[i]);
            }

            // Give the new buffer to atom.
            currentCellDataBuffer->setBufferObject(bufferMemory);
        };

    reformatBuffer("cell_data");
    reformatBuffer("cell_data_offsets");

    sight::atoms::Object::sptr pointC = previous->getAttribute<sight::atoms::Object>("point_colors");
    if(pointC)
    {
        sight::atoms::Blob::sptr pointCBuffer = pointC->getAttribute<sight::atoms::Blob>("buffer");

        if(pointCBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | sight::data::Mesh::Attributes::POINT_COLORS;
        }
    }

    sight::atoms::Object::sptr pointN = previous->getAttribute<sight::atoms::Object>("point_normals");
    if(pointN)
    {
        sight::atoms::Blob::sptr pointNBuffer = pointN->getAttribute<sight::atoms::Blob>("buffer");

        if(pointNBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | sight::data::Mesh::Attributes::POINT_NORMALS;
        }
    }

    sight::atoms::Object::sptr pointT = previous->getAttribute<sight::atoms::Object>("point_tex_coords");
    if(pointT)
    {
        sight::atoms::Blob::sptr pointTBuffer = pointT->getAttribute<sight::atoms::Blob>("buffer");

        if(pointTBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | sight::data::Mesh::Attributes::POINT_TEX_COORDS;
        }
    }

    sight::atoms::Object::sptr cellC = previous->getAttribute<sight::atoms::Object>("cell_colors");
    if(cellC)
    {
        sight::atoms::Blob::sptr cellCBuffer = cellC->getAttribute<sight::atoms::Blob>("buffer");

        if(cellCBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | sight::data::Mesh::Attributes::CELL_COLORS;
        }
    }

    sight::atoms::Object::sptr cellN = previous->getAttribute<sight::atoms::Object>("cell_normals");
    if(cellN)
    {
        sight::atoms::Blob::sptr cellNBuffer = cellN->getAttribute<sight::atoms::Blob>("buffer");

        if(cellNBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | sight::data::Mesh::Attributes::CELL_NORMALS;
        }
    }

    sight::atoms::Object::sptr cellT = previous->getAttribute<sight::atoms::Object>("cell_tex_coords");
    if(cellT)
    {
        sight::atoms::Blob::sptr cellTBuffer = cellT->getAttribute<sight::atoms::Blob>("buffer");

        if(cellTBuffer->getBufferObject()->getBuffer())
        {
            meshAttributes = meshAttributes | sight::data::Mesh::Attributes::CELL_TEX_COORDS;
        }
    }

    // Create helper
    io::atoms::patch::helper::Object helper(current);
    helper.addAttribute("attributes", sight::atoms::Numeric::New<int>(static_cast<int>(meshAttributes)));
}

} // namespace Mesh

} // namespace data

} // namespace sight::io::patch::structural
