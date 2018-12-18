/************************************************************************
 *
 * Copyright (C) 2015-2016 IRCAD France
 * Copyright (C) 2015-2016 IHU Strasbourg
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

#include "fwStructuralPatch/fwData/Material/V3ToV4.hpp"

#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>

namespace fwStructuralPatch
{

namespace fwData
{

namespace Material
{

V3ToV4::V3ToV4() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Material";
    m_targetClassname = "::fwData::Material";
    m_originVersion   = "3";
    m_targetVersion   = "4";
}

// ----------------------------------------------------------------------------

V3ToV4::~V3ToV4()
{
}

// ----------------------------------------------------------------------------

V3ToV4::V3ToV4( const V3ToV4 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V3ToV4::apply( const ::fwAtoms::Object::sptr& previous,
                    const ::fwAtoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    ::fwAtomsPatch::helper::Object helper(current);

    // Retrieve and remove lighting attribute, then update shading mode
    ::fwAtoms::Boolean::sptr lighting = previous->getAttribute< ::fwAtoms::Boolean >("lighting");
    bool bLighting = lighting->getValue();

    helper.removeAttribute("lighting");

    if(!bLighting)
    {
        helper.replaceAttribute("shading_mode", ::fwAtoms::String::New("AMBIENT"));
    }

    // Switch diffuse and ambient
    ::fwAtoms::Object::sptr ambient = current->getAttribute< ::fwAtoms::Object >("ambient");
    ::fwAtoms::Object::sptr diffuse = current->getAttribute< ::fwAtoms::Object >("diffuse");

    // Replace diffuse by previous ambient
    helper.replaceAttribute("diffuse", ambient);
    helper.replaceAttribute("ambient", diffuse);

    // Set ambient to 0.05f

    ::fwAtoms::Sequence::sptr ambientSeq = diffuse->getAttribute< ::fwAtoms::Sequence >("rgba");
    ::fwAtoms::Numeric::sptr r           = std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[0]);
    r->setFromString(".05");
    ::fwAtoms::Numeric::sptr g = std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[1]);
    g->setFromString(".05");
    ::fwAtoms::Numeric::sptr b = std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[2]);
    b->setFromString(".05");
    ::fwAtoms::Numeric::sptr a = std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[3]);
    a->setFromString("1.");
}

} // namespace Material

} // namespace fwData

} // namespace fwStructuralPatch
