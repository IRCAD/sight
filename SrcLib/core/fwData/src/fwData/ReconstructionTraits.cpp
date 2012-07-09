/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/ReconstructionTraits.hpp"
#include "fwData/registry/macros.hpp"

fwDataRegisterMacro( ::fwData::ReconstructionTraits );

namespace fwData
{

//------------------------------------------------------------------------------

void ReconstructionTraits::setMaskOpNode( ::fwData::Node::sptr maskOpNode )
{
    m_maskOpNode = maskOpNode;
}

//------------------------------------------------------------------------------

::fwData::Node::sptr ReconstructionTraits::getMaskOpNode()
{

    return m_maskOpNode;
}

//------------------------------------------------------------------------------

void ReconstructionTraits::setMeshOpNode( ::fwData::Node::sptr meshOpNode )
{
    m_meshOpNode =  meshOpNode;
}

//------------------------------------------------------------------------------

::fwData::Node::sptr ReconstructionTraits::getMeshOpNode()
{

    return m_meshOpNode;
}

//------------------------------------------------------------------------------

void ReconstructionTraits::setStructureTraits( ::fwData::StructureTraits::sptr structureTraits )
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
{}


//------------------------------------------------------------------------------

ReconstructionTraits::~ReconstructionTraits()
{}

//------------------------------------------------------------------------------

} // namespace fwData


