/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwStructuralPatch/creator/fwData/PointList1.hpp"

#include <fwAtoms/Sequence.hpp>

#include <fwAtomsPatch/helper/Object.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace fwData
{

// ----------------------------------------------------------------------------

PointList1::PointList1()
{
    m_classname = "::fwData::PointList";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

PointList1::~PointList1()
{
}

// ----------------------------------------------------------------------------

PointList1::PointList1( const PointList1& cpy ) :
    ::fwAtomsPatch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr PointList1::create()
{
    ::fwAtoms::Object::sptr pl = this->createObjBase();
    ::fwAtomsPatch::helper::Object helper(pl);

    helper.addAttribute("points", ::fwAtoms::Sequence::New());

    return pl;
}

// ----------------------------------------------------------------------------

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
