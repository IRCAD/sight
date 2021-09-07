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

#include "io/patch/semantic/V1/V2/data/Acquisition.hpp"

#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <core/tools/UUID.hpp>

#include <io/atoms/patch/helper/functions.hpp>

namespace sight::io::patch::semantic
{

namespace V1
{

namespace V2
{

namespace data
{

Acquisition::Acquisition() :
    io::atoms::patch::ISemanticPatch()
{
    m_originClassname = "::sight::data::Acquisition";
    m_originVersion   = "1";
    this->addContext("MedicalData", "V1", "V2");
}

// ----------------------------------------------------------------------------

Acquisition::~Acquisition()
{
}

// ----------------------------------------------------------------------------

Acquisition::Acquisition(const Acquisition& cpy) :
    io::atoms::patch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Acquisition::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    ISemanticPatch::apply(previous, current, newVersions);
    io::atoms::patch::helper::cleanFields(current);

    sight::atoms::Object::sptr image = current->getAttribute<sight::atoms::Object>("image");
    sight::atoms::Map::sptr fields   = image->getAttribute<sight::atoms::Map>("fields");

    sight::atoms::Map::ConstIteratorType it = fields->find("m_commentId");
    io::atoms::patch::helper::Object helper(current);
    if(it != fields->end())
    {
        sight::atoms::Object::sptr value = sight::atoms::Object::dynamicCast(it->second);
        helper.replaceAttribute("description", sight::atoms::String::New(value->getAttribute("value")->getString()));
    }

    sight::atoms::String::sptr uid = current->getAttribute<sight::atoms::String>("instance_uid");
    if(uid->getValue().empty())
    {
        helper.replaceAttribute("instance_uid", sight::atoms::String::New(core::tools::UUID::generateUUID()));
    }
}

// ----------------------------------------------------------------------------

} // namespace data

} // namespace V2

} // namespace V1

} // namespace sight::io::patch::semantic
