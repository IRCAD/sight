/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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


