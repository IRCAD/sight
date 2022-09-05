/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <data/Line.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtlPositionMessage.h>

#include <algorithm>

namespace sight::io::igtl::detail::converter
{

const std::string LineConverter::s_IGTL_TYPE          = "POSITION";
const std::string LineConverter::s_FWDATA_OBJECT_TYPE = data::Line::classname();

converterRegisterMacro(io::igtl::detail::converter::LineConverter);

LineConverter::LineConverter()
= default;

//-----------------------------------------------------------------------------

LineConverter::~LineConverter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer LineConverter::fromFwDataObject(data::Object::csptr src) const
{
    std::array<float, 3> pos {};
    std::array<float, 4> direction {};

    ::igtl::PositionMessage::Pointer dest;
    data::Line::csptr srcLine = data::Line::dynamicConstCast(src);

    dest = ::igtl::PositionMessage::New();
    std::transform(
        srcLine->getPosition()->getCoord().begin(),
        srcLine->getPosition()->getCoord().end(),
        pos.data(),
        boost::numeric_cast<double, float>
    );
    std::transform(
        srcLine->getDirection()->getCoord().begin(),
        srcLine->getDirection()->getCoord().end(),
        direction.data(),
        boost::numeric_cast<double, float>
    );
    dest->SetPosition(pos.data());
    dest->SetQuaternion(direction.data()); // We use the quaternion to store the direction
    return {dest.GetPointer()};
}

//-----------------------------------------------------------------------------

data::Object::sptr LineConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    std::array<float, 3> igtlPos {};
    // While we only use the first three elements of direction, igtl::PositionMessage::GetQuaternion requires a
    // four-element array.
    std::array<float, 4> igtlDirection {};

    data::Line::sptr dest                    = data::Line::New();
    auto* msg                                = dynamic_cast< ::igtl::PositionMessage*>(src.GetPointer());
    ::igtl::PositionMessage::Pointer srcLine = ::igtl::PositionMessage::Pointer(msg);
    dest->setPosition(data::Point::New());
    dest->setDirection(data::Point::New());
    srcLine->GetPosition(igtlPos.data());
    srcLine->GetQuaternion(igtlDirection.data());
    std::transform(
        igtlPos.begin(),
        igtlPos.end(),
        dest->getPosition()->getCoord().begin(),
        boost::numeric_cast<float, double>
    );
    std::transform(
        igtlDirection.begin(),
        igtlDirection.begin() + 3,
        dest->getDirection()->getCoord().begin(),
        boost::numeric_cast<float, double>
    );

    return dest;
}

//-----------------------------------------------------------------------------

IConverter::sptr LineConverter::New()
{
    return std::make_shared<LineConverter>();
}

//-----------------------------------------------------------------------------

std::string const& LineConverter::getIgtlType() const
{
    return LineConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& LineConverter::getFwDataObjectType() const
{
    return LineConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
