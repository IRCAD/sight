/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/ROITraits.hpp"

fwDataRegisterMacro( ::fwData::ROITraits );

namespace fwData
{

//------------------------------------------------------------------------------

ROITraits::ROITraits(::fwData::Object::Key key)
{}

//------------------------------------------------------------------------------

ROITraits::~ROITraits()
{}

//------------------------------------------------------------------------------

void ROITraits::setMaskOpNode( ::fwData::Node::sptr maskOpNode )
{
    m_maskOpNode = maskOpNode;
}

//------------------------------------------------------------------------------

::fwData::Node::sptr ROITraits::getMaskOpNode()
{
    ::fwData::Node::sptr opNode;
    if ( m_evaluatedExp != "W" ) // Thus mask op node must be assigned
    {
        opNode = m_maskOpNode;
    }
    return opNode;
}

//------------------------------------------------------------------------------

void ROITraits::setStructureTraits( ::fwData::StructureTraits::sptr structureTraits )
{
    m_structureTraits = structureTraits;
}

//------------------------------------------------------------------------------

::fwData::StructureTraits::sptr ROITraits::getStructureTraits()
{
    return m_structureTraits;
}

//------------------------------------------------------------------------------
} // namespace fwData


