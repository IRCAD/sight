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

#include <fwTools/UUID.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>

#include <fwAtomsPatch/helper/functions.hpp>

#include "fwMDSemanticPatch/V2/V03AGO/fwData/Image.hpp"

namespace fwMDSemanticPatch
{
namespace V2
{
namespace V03AGO
{
namespace fwData
{

Image::Image() : ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::fwData::Image";
    m_originVersion   = "1";
    this->addContext("MedicalData", "V2", "V03AGO");
}

// ----------------------------------------------------------------------------

Image::~Image()
{
}

// ----------------------------------------------------------------------------

Image::Image( const Image &cpy ) : ::fwAtomsPatch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Image::apply(
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    ::fwAtomsPatch::helper::cleanFields( current );
    ::fwAtomsPatch::helper::Object helper( current );

    ::fwAtoms::Object::sptr array        = ::fwAtoms::Object::dynamicCast(previous->getAttribute("array"));
    ::fwAtoms::Numeric::sptr nbComponent = ::fwAtoms::Numeric::dynamicCast(array->getAttribute("nb_of_components"));

    helper.replaceAttribute("nb_components", nbComponent->clone());
}

// ----------------------------------------------------------------------------

} // namespace fwData
} // namespace V03AGO
} // namespace V2
} // namespace fwMDSemanticPatch

