/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/ROITraits.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA(sight::data::ROITraits);

namespace sight::data
{

//------------------------------------------------------------------------------

ROITraits::ROITraits(data::Object::Key /*unused*/)
{
}

//------------------------------------------------------------------------------

void ROITraits::setMaskOpNode(const data::Node::sptr& maskOpNode)
{
    m_maskOpNode = maskOpNode;
}

//------------------------------------------------------------------------------

data::Node::sptr ROITraits::getMaskOpNode()
{
    data::Node::sptr opNode;
    if(m_evaluatedExp != "W") // Thus mask op node must be assigned
    {
        opNode = m_maskOpNode;
    }

    return opNode;
}

//------------------------------------------------------------------------------

data::Node::csptr ROITraits::getMaskOpNode() const
{
    data::Node::csptr opNode;
    if(m_evaluatedExp != "W") // Thus mask op node must be assigned
    {
        opNode = m_maskOpNode;
    }

    return opNode;
}

//------------------------------------------------------------------------------

void ROITraits::setStructureTraits(const data::StructureTraits::sptr& structureTraits)
{
    m_structureTraits = structureTraits;
}

//------------------------------------------------------------------------------

data::StructureTraits::sptr ROITraits::getStructureTraits()
{
    return m_structureTraits;
}

//------------------------------------------------------------------------------

data::StructureTraits::csptr ROITraits::getStructureTraits() const
{
    return m_structureTraits;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void ROITraits::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_identifier      = other->m_identifier;
    m_evaluatedExp    = other->m_evaluatedExp;
    m_maskOpNode      = other->m_maskOpNode;
    m_structureTraits = other->m_structureTraits;

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

void ROITraits::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_identifier      = other->m_identifier;
    m_evaluatedExp    = other->m_evaluatedExp;
    m_maskOpNode      = data::Object::copy(other->m_maskOpNode, cache);
    m_structureTraits = data::Object::copy(other->m_structureTraits, cache);

    BaseClass::deepCopy(other, cache);
}

//------------------------------------------------------------------------------

bool ROITraits::operator==(const ROITraits& other) const noexcept
{
    if(m_identifier != other.m_identifier
       || m_evaluatedExp != other.m_evaluatedExp
       || !core::tools::is_equal(m_maskOpNode, other.m_maskOpNode)
       || !core::tools::is_equal(m_structureTraits, other.m_structureTraits))
    {
        return false;
    }

    // Super class last
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool ROITraits::operator!=(const ROITraits& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
