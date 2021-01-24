/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

fwDataRegisterMacro( sight::data::ReconstructionTraits );

namespace sight::data
{

//------------------------------------------------------------------------------

void ReconstructionTraits::setMaskOpNode(const data::Node::sptr& maskOpNode )
{
    m_maskOpNode = maskOpNode;
}

//------------------------------------------------------------------------------

data::Node::sptr ReconstructionTraits::getMaskOpNode()
{

    return m_maskOpNode;
}

//------------------------------------------------------------------------------

void ReconstructionTraits::setMeshOpNode(const data::Node::sptr& meshOpNode )
{
    m_meshOpNode = meshOpNode;
}

//------------------------------------------------------------------------------

data::Node::sptr ReconstructionTraits::getMeshOpNode()
{
    return m_meshOpNode;
}

//------------------------------------------------------------------------------

void ReconstructionTraits::setStructureTraits(const data::StructureTraits::sptr& structureTraits )
{
    m_structureTraits = structureTraits;
}

//------------------------------------------------------------------------------

data::StructureTraits::sptr ReconstructionTraits::getStructureTraits()
{
    return m_structureTraits;
}

//------------------------------------------------------------------------------

ReconstructionTraits::ReconstructionTraits(data::Object::Key)
{
}

//------------------------------------------------------------------------------

ReconstructionTraits::~ReconstructionTraits()
{
}

//------------------------------------------------------------------------------

void ReconstructionTraits::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    ReconstructionTraits::csptr other = ReconstructionTraits::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );

    SLM_FATAL("Not implemented." );
}

} // namespace sight::data
