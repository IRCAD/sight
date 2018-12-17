/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwData/ReconstructionTraits.hpp"
#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::ReconstructionTraits );

namespace fwData
{

//------------------------------------------------------------------------------

void ReconstructionTraits::setMaskOpNode(const ::fwData::Node::sptr& maskOpNode )
{
    m_maskOpNode = maskOpNode;
}

//------------------------------------------------------------------------------

::fwData::Node::sptr ReconstructionTraits::getMaskOpNode()
{

    return m_maskOpNode;
}

//------------------------------------------------------------------------------

void ReconstructionTraits::setMeshOpNode(const ::fwData::Node::sptr& meshOpNode )
{
    m_meshOpNode = meshOpNode;
}

//------------------------------------------------------------------------------

::fwData::Node::sptr ReconstructionTraits::getMeshOpNode()
{
    return m_meshOpNode;
}

//------------------------------------------------------------------------------

void ReconstructionTraits::setStructureTraits(const ::fwData::StructureTraits::sptr& structureTraits )
{
    m_structureTraits = structureTraits;
}

//------------------------------------------------------------------------------

::fwData::StructureTraits::sptr ReconstructionTraits::getStructureTraits()
{
    return m_structureTraits;
}

//------------------------------------------------------------------------------

ReconstructionTraits::ReconstructionTraits(::fwData::Object::Key key)
{
}


//------------------------------------------------------------------------------

ReconstructionTraits::~ReconstructionTraits()
{
}

//------------------------------------------------------------------------------

void ReconstructionTraits::cachedDeepCopy(const Object::csptr &source, DeepCopyCacheType &cache)
{
    ReconstructionTraits::csptr other = ReconstructionTraits::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );

    SLM_FATAL("Not implemented." );
}

} // namespace fwData


