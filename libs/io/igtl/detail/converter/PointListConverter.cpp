/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/igtl/detail/converter/PointListConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"

#include <data/PointList.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtlPointMessage.h>

#include <algorithm>

namespace sight::io::igtl::detail
{

namespace converter
{

const std::string PointListConverter::s_IGTL_TYPE          = "POINT";
const std::string PointListConverter::s_FWDATA_OBJECT_TYPE = data::PointList::classname();

converterRegisterMacro(io::igtl::detail::converter::PointListConverter);

PointListConverter::PointListConverter()
{
}

//-----------------------------------------------------------------------------

PointListConverter::~PointListConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer PointListConverter::fromFwDataObject(data::Object::csptr src) const
{
    float pos[3];
    ::igtl::PointElement::Pointer elem;
    data::PointList::csptr srcPoints = data::PointList::dynamicConstCast(src);

    ::igtl::PointMessage::Pointer dest = ::igtl::PointMessage::New();
    for(data::Point::sptr const& srcPoint : srcPoints->getPoints())
    {
        std::transform(
            srcPoint->getCoord().begin(),
            srcPoint->getCoord().end(),
            pos,
            ::boost::numeric_cast<double, float>
        );
        elem = ::igtl::PointElement::New();
        elem->SetPosition(pos);
        dest->AddPointElement(elem);
    }

    return ::igtl::MessageBase::Pointer(dest.GetPointer());
}

//-----------------------------------------------------------------------------

data::Object::sptr PointListConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    float igtlPos[3];
    ::igtl::PointElement::Pointer elem;
    std::vector<data::Point::sptr> fwPoints;
    data::Point::sptr fwPoint;

    ::igtl::PointMessage* msg               = dynamic_cast< ::igtl::PointMessage*>(src.GetPointer());
    ::igtl::PointMessage::Pointer srcPoints = ::igtl::PointMessage::Pointer(msg);
    data::PointList::sptr dest              = data::PointList::New();
    for(int i = 0 ; i < srcPoints->GetNumberOfPointElement() ; ++i)
    {
        fwPoint = data::Point::New();
        srcPoints->GetPointElement(i, elem);
        elem->GetPosition(igtlPos);
        std::transform(
            &igtlPos[0],
            &igtlPos[3],
            fwPoint->getCoord().begin(),
            ::boost::numeric_cast<float, double>
        );
        fwPoints.push_back(fwPoint);
    }

    dest->setPoints(fwPoints);

    return dest;
}

//-----------------------------------------------------------------------------

IConverter::sptr PointListConverter::New()
{
    return std::make_shared<PointListConverter>();
}

//-----------------------------------------------------------------------------

std::string const& PointListConverter::getIgtlType() const
{
    return PointListConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& PointListConverter::getFwDataObjectType() const
{
    return PointListConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter

} // namespace sight::io::igtl::detail}
