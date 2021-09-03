/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

ROITraits::ROITraits(data::Object::Key)
{
}

//------------------------------------------------------------------------------

ROITraits::~ROITraits()
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

void ROITraits::shallowCopy(const Object::csptr& _source)
{
    ROITraits::csptr other = ROITraits::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);

    m_identifier      = other->m_identifier;
    m_evaluatedExp    = other->m_evaluatedExp;
    m_maskOpNode      = other->m_maskOpNode;
    m_structureTraits = other->m_structureTraits;
}

//------------------------------------------------------------------------------

void ROITraits::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    ROITraits::csptr other = ROITraits::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);

    m_identifier      = other->m_identifier;
    m_evaluatedExp    = other->m_evaluatedExp;
    m_maskOpNode      = data::Object::copy(other->m_maskOpNode, cache);
    m_structureTraits = data::Object::copy(other->m_structureTraits, cache);
}

} // namespace sight::data
