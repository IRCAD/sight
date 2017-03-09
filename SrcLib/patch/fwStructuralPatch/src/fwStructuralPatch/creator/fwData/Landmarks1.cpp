/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwStructuralPatch/creator/fwData/Landmarks1.hpp"

#include <fwAtoms/Map.hpp>

#include <fwAtomsPatch/helper/Object.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace fwData
{

// ----------------------------------------------------------------------------

Landmarks1::Landmarks1()
{
    m_classname = "::fwData::Landmarks";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Landmarks1::~Landmarks1()
{
}

// ----------------------------------------------------------------------------

Landmarks1::Landmarks1( const Landmarks1& cpy ) :
    ::fwAtomsPatch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr Landmarks1::create()
{
    ::fwAtoms::Object::sptr landmarks = this->createObjBase();
    ::fwAtomsPatch::helper::Object helper(landmarks);

    helper.addAttribute("landmarks", ::fwAtoms::Map::New());

    return landmarks;
}

// ----------------------------------------------------------------------------

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
