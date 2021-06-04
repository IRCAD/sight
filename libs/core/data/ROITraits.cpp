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

void ROITraits::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    ROITraits::csptr other = ROITraits::dynamicConstCast(source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(source, cache);

    SIGHT_FATAL("Not implemented.");
}

} // namespace sight::data
