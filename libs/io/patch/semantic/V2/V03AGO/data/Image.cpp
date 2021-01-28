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

#include "io/patch/semantic/V2/V03AGO/data/Image.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>

#include <core/tools/UUID.hpp>

#include <io/atoms/patch/helper/functions.hpp>

namespace sight::io::patch::semantic
{
namespace V2
{
namespace V03AGO
{
namespace data
{

Image::Image() :
    io::atoms::patch::ISemanticPatch()
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
    io::atoms::patch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Image::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    io::atoms::patch::helper::cleanFields( current );
    io::atoms::patch::helper::Object helper( current );

    sight::atoms::Object::sptr array        = sight::atoms::Object::dynamicCast(previous->getAttribute("array"));
    sight::atoms::Numeric::sptr nbComponent =
        sight::atoms::Numeric::dynamicCast(array->getAttribute("nb_of_components"));

    helper.replaceAttribute("nb_components", nbComponent->clone());
}

// ----------------------------------------------------------------------------

} // namespace data
} // namespace V03AGO
} // namespace V2
} // namespace sight::io::patch::semantic
