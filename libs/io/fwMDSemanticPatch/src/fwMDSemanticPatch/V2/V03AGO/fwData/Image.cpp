/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwMDSemanticPatch/V2/V03AGO/data/Image.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>

#include <core/tools/UUID.hpp>

#include <fwAtomsPatch/helper/functions.hpp>

namespace fwMDSemanticPatch
{
namespace V2
{
namespace V03AGO
{
namespace sight::data
{

Image::Image() :
    ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::sight::data::Image";
    m_originVersion   = "1";
    this->addContext("MedicalData", "V2", "V03AGO");
}

// ----------------------------------------------------------------------------

Image::~Image()
{
}

// ----------------------------------------------------------------------------

Image::Image( const Image& cpy ) :
    ::fwAtomsPatch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Image::apply(
    const atoms::Object::sptr& previous,
    const atoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    ::fwAtomsPatch::helper::cleanFields( current );
    ::fwAtomsPatch::helper::Object helper( current );

    atoms::Object::sptr array        = atoms::Object::dynamicCast(previous->getAttribute("array"));
    atoms::Numeric::sptr nbComponent = atoms::Numeric::dynamicCast(array->getAttribute("nb_of_components"));

    helper.replaceAttribute("nb_components", nbComponent->clone());
}

// ----------------------------------------------------------------------------

} // namespace sight::data
} // namespace V03AGO
} // namespace V2
} // namespace fwMDSemanticPatch
