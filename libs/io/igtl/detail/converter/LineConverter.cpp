/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/igtl/detail/converter/LineConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"

#include <data/line.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtlPositionMessage.h>

#include <algorithm>

namespace sight::io::igtl::detail::converter
{

const std::string LineConverter::s_IGTL_TYPE          = "POSITION";
const std::string LineConverter::s_FWDATA_OBJECT_TYPE = data::line::classname();

CONVERTER_REGISTER_MACRO(io::igtl::detail::converter::LineConverter);

LineConverter::LineConverter()
= default;

//-----------------------------------------------------------------------------

LineConverter::~LineConverter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer LineConverter::fromFwDataObject(data::object::csptr _src) const
{
    std::array<float, 3> pos {};
    std::array<float, 4> direction {};

    ::igtl::PositionMessage::Pointer dest;
    data::line::csptr src_line = std::dynamic_pointer_cast<const data::line>(_src);

    dest = ::igtl::PositionMessage::New();
    std::transform(
        src_line->getPosition()->getCoord().begin(),
        src_line->getPosition()->getCoord().end(),
        pos.data(),
        boost::numeric_cast<double, float>
    );
    std::transform(
        src_line->getDirection()->getCoord().begin(),
        src_line->getDirection()->getCoord().end(),
        direction.data(),
        boost::numeric_cast<double, float>
    );
    dest->SetPosition(pos.data());
    dest->SetQuaternion(direction.data()); // We use the quaternion to store the direction
    return {dest.GetPointer()};
}

//-----------------------------------------------------------------------------

data::object::sptr LineConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer _src) const
{
    std::array<float, 3> igtl_pos {};
    // While we only use the first three elements of direction, igtl::PositionMessage::GetQuaternion requires a
    // four-element array.
    std::array<float, 4> igtl_direction {};

    data::line::sptr dest                     = std::make_shared<data::line>();
    auto* msg                                 = dynamic_cast< ::igtl::PositionMessage*>(_src.GetPointer());
    ::igtl::PositionMessage::Pointer src_line = ::igtl::PositionMessage::Pointer(msg);
    dest->setPosition(std::make_shared<data::point>());
    dest->setDirection(std::make_shared<data::point>());
    src_line->GetPosition(igtl_pos.data());
    src_line->GetQuaternion(igtl_direction.data());
    std::transform(
        igtl_pos.begin(),
        igtl_pos.end(),
        dest->getPosition()->getCoord().begin(),
        boost::numeric_cast<float, double>
    );
    std::transform(
        igtl_direction.begin(),
        igtl_direction.begin() + 3,
        dest->getDirection()->getCoord().begin(),
        boost::numeric_cast<float, double>
    );

    return dest;
}

//-----------------------------------------------------------------------------

base::sptr LineConverter::New()
{
    return std::make_shared<LineConverter>();
}

//-----------------------------------------------------------------------------

std::string const& LineConverter::get_igtl_type() const
{
    return LineConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& LineConverter::getFwDataObjectType() const
{
    return LineConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
