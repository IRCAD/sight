/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "data/ReconstructionTraits.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA(sight::data::ReconstructionTraits);

namespace sight::data
{

//------------------------------------------------------------------------------

void ReconstructionTraits::setMaskOpNode(const data::Node::sptr& maskOpNode)
{
    m_maskOpNode = maskOpNode;
}

//------------------------------------------------------------------------------

data::Node::sptr ReconstructionTraits::getMaskOpNode()
{
    return m_maskOpNode;
}

//------------------------------------------------------------------------------

void ReconstructionTraits::setMeshOpNode(const data::Node::sptr& meshOpNode)
{
    m_meshOpNode = meshOpNode;
}

//------------------------------------------------------------------------------

data::Node::sptr ReconstructionTraits::getMeshOpNode()
{
    return m_meshOpNode;
}

//------------------------------------------------------------------------------

void ReconstructionTraits::setStructureTraits(const data::StructureTraits::sptr& structureTraits)
{
    m_structureTraits = structureTraits;
}

//------------------------------------------------------------------------------

data::StructureTraits::sptr ReconstructionTraits::getStructureTraits()
{
    return m_structureTraits;
}

//------------------------------------------------------------------------------

ReconstructionTraits::ReconstructionTraits(data::Object::Key /*unused*/)
{
}

//------------------------------------------------------------------------------

ReconstructionTraits::~ReconstructionTraits()
= default;

//------------------------------------------------------------------------------

void ReconstructionTraits::shallowCopy(const data::Object::csptr& _source)
{
    ReconstructionTraits::csptr other = ReconstructionTraits::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldShallowCopy(other);

    m_identifier      = other->m_identifier;
    m_maskOpNode      = other->m_maskOpNode;
    m_meshOpNode      = other->m_meshOpNode;
    m_structureTraits = other->m_structureTraits;
}

//------------------------------------------------------------------------------

void ReconstructionTraits::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    ReconstructionTraits::csptr other = ReconstructionTraits::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldDeepCopy(other, cache);
    m_identifier      = other->m_identifier;
    m_maskOpNode      = data::Object::copy(other->m_maskOpNode, cache);
    m_meshOpNode      = data::Object::copy(other->m_meshOpNode, cache);
    m_structureTraits = data::Object::copy(other->m_structureTraits, cache);
}

//------------------------------------------------------------------------------

bool ReconstructionTraits::operator==(const ReconstructionTraits& other) const noexcept
{
    if(m_identifier != other.m_identifier
       || !core::tools::is_equal(m_maskOpNode, other.m_maskOpNode)
       || !core::tools::is_equal(m_meshOpNode, other.m_meshOpNode)
       || !core::tools::is_equal(m_structureTraits, other.m_structureTraits))
    {
        return false;
    }

    // Super class last
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool ReconstructionTraits::operator!=(const ReconstructionTraits& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
